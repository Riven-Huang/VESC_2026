from __future__ import annotations

import csv
import math
from dataclasses import dataclass
from pathlib import Path

import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, confusion_matrix
from sklearn.model_selection import GroupShuffleSplit


ROOT = Path(__file__).resolve().parents[1]
DATASET_DIR = ROOT / "VeriHealthi_IMU_Dataset"
SDK_APP_DIR = ROOT / "VeriHealthi_QEMU_SDK_v3.7" / "galaxy_sdk" / "app"
REPORT_PATH = Path(__file__).resolve().parent / "rf_training_report.txt"

SAMPLE_RATE_HZ = 50
WINDOW_SAMPLES = 50
WINDOW_STEP_SAMPLES = 10
GESTURE_CONFIRM_WINDOWS = 2
GESTURE_OUTPUT_COOLDOWN_MS = 1600
GESTURE_RF_MIN_VOTES = 12
EVENT_MATCH_TOLERANCE_MS = 1200
POSITIVE_OFFSETS = range(-20, 21, 5)
NEGATIVE_STEP_SAMPLES = 25
NEGATIVE_MARGIN_SAMPLES = 90

LABEL_TO_ID = {
    "others": 0,
    "pinch": 1,
    "clench": 2,
    "up": 3,
    "down": 4,
}
ID_TO_LABEL = {value: key for key, value in LABEL_TO_ID.items()}
CLASS_NAMES = [ID_TO_LABEL[i] for i in range(len(ID_TO_LABEL))]

FEATURE_NAMES = [
    "range_gx",
    "range_gy",
    "range_gz",
    "range_ax",
    "range_ay",
    "range_az",
    "mean_gx",
    "mean_gy",
    "mean_gz",
    "mean_ax",
    "mean_ay",
    "mean_az",
    "delta_gx",
    "delta_gy",
    "delta_gz",
    "delta_ax",
    "delta_ay",
    "delta_az",
    "gyro_abs_mean",
    "accel_abs_mean",
    "gyro_range_sum",
    "accel_range_sum",
]


@dataclass
class LabeledWindow:
    features: list[int]
    label_id: int
    group: str


@dataclass
class EventRecord:
    timestamp_ms: int
    label_id: int


@dataclass
class PredictionRecord:
    timestamp_ms: int
    label_id: int
    votes: int


def load_raw_samples(path: Path) -> np.ndarray:
    values: list[int] = []
    for line in path.read_text(errors="ignore").splitlines()[5:]:
        line = line.strip()
        if not line:
            continue
        try:
            values.append(int(line))
        except ValueError:
            continue

    frame_count = len(values) // 7
    values = values[: frame_count * 7]
    if frame_count == 0:
        return np.empty((0, 6), dtype=np.int32)

    frames = np.array(values, dtype=np.int64).reshape(frame_count, 7)
    return frames[:, :6].astype(np.int32)


def load_event_indices(path: Path) -> list[int]:
    events: list[int] = []
    with path.open(newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            event_time_s = float(row["event_time_s"])
            events.append(int(round(event_time_s * SAMPLE_RATE_HZ)))
    return events


def load_events(path: Path) -> list[EventRecord]:
    events: list[EventRecord] = []
    label_id = LABEL_TO_ID[path.parent.name]
    with path.open(newline="") as handle:
        reader = csv.DictReader(handle)
        for row in reader:
            event_time_s = float(row["event_time_s"])
            events.append(EventRecord(int(round(event_time_s * 1000)), label_id))
    return events


def csv_to_txt_path(csv_path: Path) -> Path | None:
    candidate = csv_path.with_name(f"IMU_{csv_path.stem}.txt")
    if candidate.exists():
        return candidate

    matches = list(csv_path.parent.glob(f"IMU_{csv_path.stem}*.txt"))
    if matches:
        return matches[0]
    return None


def extract_features(window: np.ndarray) -> list[int]:
    axis_min = window.min(axis=0)
    axis_max = window.max(axis=0)
    axis_range = axis_max - axis_min
    axis_mean = np.rint(window.mean(axis=0)).astype(np.int32)
    delta = window[-1] - window[0]
    gyro_abs_mean = int(np.rint(np.abs(window[:, :3]).sum(axis=1).mean()))
    accel_abs_mean = int(np.rint(np.abs(window[:, 3:6]).sum(axis=1).mean()))
    gyro_range_sum = int(axis_range[:3].sum())
    accel_range_sum = int(axis_range[3:6].sum())

    features = [
        *axis_range.tolist(),
        *axis_mean.tolist(),
        *delta.tolist(),
        gyro_abs_mean,
        accel_abs_mean,
        gyro_range_sum,
        accel_range_sum,
    ]
    return [int(value) for value in features]


def window_at(samples: np.ndarray, center_index: int) -> np.ndarray | None:
    start = center_index - WINDOW_SAMPLES // 2
    end = start + WINDOW_SAMPLES
    if start < 0 or end > len(samples):
        return None
    return samples[start:end]


def far_from_events(center_index: int, event_indices: list[int]) -> bool:
    return all(abs(center_index - event) >= NEGATIVE_MARGIN_SAMPLES for event in event_indices)


def collect_labeled_windows() -> list[LabeledWindow]:
    windows: list[LabeledWindow] = []

    for label in ["pinch", "clench", "up", "down"]:
        label_dir = DATASET_DIR / label
        for csv_path in label_dir.glob("*.csv"):
            txt_path = csv_to_txt_path(csv_path)
            if txt_path is None:
                continue
            samples = load_raw_samples(txt_path)
            event_indices = load_event_indices(csv_path)
            group = f"{label}/{txt_path.stem}"

            for event_index in event_indices:
                for offset in POSITIVE_OFFSETS:
                    window = window_at(samples, event_index + offset)
                    if window is not None:
                        windows.append(
                            LabeledWindow(extract_features(window), LABEL_TO_ID[label], group)
                        )

            for center in range(WINDOW_SAMPLES // 2, len(samples) - WINDOW_SAMPLES // 2, NEGATIVE_STEP_SAMPLES):
                if far_from_events(center, event_indices):
                    window = window_at(samples, center)
                    if window is not None:
                        windows.append(
                            LabeledWindow(extract_features(window), LABEL_TO_ID["others"], group)
                        )

    others_dir = DATASET_DIR / "others"
    for txt_path in others_dir.glob("*.txt"):
        samples = load_raw_samples(txt_path)
        group = f"others/{txt_path.stem}"
        for center in range(WINDOW_SAMPLES // 2, len(samples) - WINDOW_SAMPLES // 2, NEGATIVE_STEP_SAMPLES):
            window = window_at(samples, center)
            if window is not None:
                windows.append(LabeledWindow(extract_features(window), LABEL_TO_ID["others"], group))

    return windows


def train_model(windows: list[LabeledWindow]) -> tuple[RandomForestClassifier, str, set[str]]:
    x = np.array([window.features for window in windows], dtype=np.int32)
    y = np.array([window.label_id for window in windows], dtype=np.int32)
    groups = np.array([window.group for window in windows])

    splitter = GroupShuffleSplit(n_splits=1, test_size=0.2, random_state=2026)
    train_index, test_index = next(splitter.split(x, y, groups))

    clf = RandomForestClassifier(
        n_estimators=25,
        max_depth=8,
        min_samples_leaf=8,
        class_weight="balanced_subsample",
        random_state=2026,
        n_jobs=-1,
    )
    clf.fit(x[train_index], y[train_index])

    prediction = clf.predict(x[test_index])
    report_lines = [
        "Random forest gesture model",
        f"windows: {len(windows)}",
        f"features: {len(FEATURE_NAMES)}",
        f"train windows: {len(train_index)}",
        f"test windows: {len(test_index)}",
        "",
        "class distribution:",
    ]
    for label_id in range(len(CLASS_NAMES)):
        report_lines.append(f"  {CLASS_NAMES[label_id]}: {int(np.sum(y == label_id))}")
    report_lines.extend(
        [
            "",
            "classification report:",
            classification_report(y[test_index], prediction, target_names=CLASS_NAMES, digits=4),
            "confusion matrix rows=true cols=pred:",
            str(confusion_matrix(y[test_index], prediction, labels=list(range(len(CLASS_NAMES))))),
            "",
            "feature importance:",
        ]
    )
    for name, importance in sorted(
        zip(FEATURE_NAMES, clf.feature_importances_), key=lambda item: item[1], reverse=True
    ):
        report_lines.append(f"  {name}: {importance:.6f}")

    test_groups = set(groups[test_index].tolist())
    return clf, "\n".join(report_lines) + "\n", test_groups


def predict_with_votes(clf: RandomForestClassifier, features: list[int]) -> tuple[int, int]:
    sample = np.array([features], dtype=np.int32)
    votes = [0] * len(CLASS_NAMES)

    for estimator in clf.estimators_:
        label_id = int(estimator.predict(sample)[0])
        votes[label_id] += 1

    best_label = max(range(len(votes)), key=lambda index: votes[index])
    return best_label, votes[best_label]


def predict_windows_with_votes(clf: RandomForestClassifier, features: np.ndarray) -> tuple[np.ndarray, np.ndarray]:
    if len(features) == 0:
        return np.empty((0,), dtype=np.int32), np.empty((0,), dtype=np.int32)

    votes = np.zeros((len(features), len(CLASS_NAMES)), dtype=np.int16)
    for estimator in clf.estimators_:
        labels = estimator.predict(features).astype(np.int32)
        votes[np.arange(len(features)), labels] += 1

    labels = votes.argmax(axis=1).astype(np.int32)
    best_votes = votes.max(axis=1).astype(np.int32)
    return labels, best_votes


def simulate_file_predictions(clf: RandomForestClassifier, samples: np.ndarray) -> list[PredictionRecord]:
    predictions: list[PredictionRecord] = []
    pending_label = LABEL_TO_ID["others"]
    pending_count = 0
    last_output_ms = -GESTURE_OUTPUT_COOLDOWN_MS
    windows: list[list[int]] = []
    timestamps_ms: list[int] = []

    for end in range(WINDOW_SAMPLES - 1, len(samples), WINDOW_STEP_SAMPLES):
        start = end + 1 - WINDOW_SAMPLES
        window = samples[start : end + 1]
        windows.append(extract_features(window))
        timestamps_ms.append(int(round(end * 1000 / SAMPLE_RATE_HZ)))

    labels, best_votes = predict_windows_with_votes(clf, np.array(windows, dtype=np.int32))

    for label_id, votes, timestamp_ms in zip(labels, best_votes, timestamps_ms):
        if label_id == LABEL_TO_ID["others"] or votes < GESTURE_RF_MIN_VOTES:
            pending_label = LABEL_TO_ID["others"]
            pending_count = 0
            continue

        if label_id == pending_label:
            pending_count += 1
        else:
            pending_label = label_id
            pending_count = 1

        if pending_count < GESTURE_CONFIRM_WINDOWS:
            continue

        if timestamp_ms - last_output_ms < GESTURE_OUTPUT_COOLDOWN_MS:
            continue

        predictions.append(PredictionRecord(timestamp_ms, label_id, votes))
        last_output_ms = timestamp_ms
        pending_count = 0

    return predictions


def group_to_paths(group: str) -> tuple[Path | None, Path | None]:
    label, stem = group.split("/", 1)
    txt_path = DATASET_DIR / label / f"{stem}.txt"
    csv_path: Path | None = None

    if label != "others":
        if stem.startswith("IMU_"):
            csv_stem = stem[4:]
        else:
            csv_stem = stem
        candidate = DATASET_DIR / label / f"{csv_stem}.csv"
        if candidate.exists():
            csv_path = candidate

    return (txt_path if txt_path.exists() else None), csv_path


def score_events(events: list[EventRecord], predictions: list[PredictionRecord]) -> dict[str, int]:
    matched_events: set[int] = set()
    matched_predictions: set[int] = set()
    stats = {"tp": 0, "wrong": 0, "duplicate": 0, "fp": 0, "fn": 0}

    for pred_index, prediction in enumerate(predictions):
        candidates = [
            (abs(prediction.timestamp_ms - event.timestamp_ms), event_index, event)
            for event_index, event in enumerate(events)
            if abs(prediction.timestamp_ms - event.timestamp_ms) <= EVENT_MATCH_TOLERANCE_MS
        ]
        if not candidates:
            continue

        _, event_index, event = min(candidates, key=lambda item: item[0])
        matched_predictions.add(pred_index)
        if event_index in matched_events:
            stats["duplicate"] += 1
        elif event.label_id == prediction.label_id:
            matched_events.add(event_index)
            stats["tp"] += 1
        else:
            matched_events.add(event_index)
            stats["wrong"] += 1

    stats["fn"] = len(events) - len(matched_events)
    stats["fp"] = len(predictions) - len(matched_predictions)
    return stats


def evaluate_event_level(clf: RandomForestClassifier, test_groups: set[str]) -> str:
    per_class = {
        label_id: {"tp": 0, "wrong": 0, "duplicate": 0, "fp": 0, "fn": 0}
        for label_id in range(len(CLASS_NAMES))
    }
    totals = {"tp": 0, "wrong": 0, "duplicate": 0, "fp": 0, "fn": 0}
    file_count = 0

    for group in sorted(test_groups):
        txt_path, csv_path = group_to_paths(group)
        if txt_path is None:
            continue

        samples = load_raw_samples(txt_path)
        predictions = simulate_file_predictions(clf, samples)
        if csv_path is None:
            stats = {"tp": 0, "wrong": 0, "duplicate": 0, "fp": len(predictions), "fn": 0}
            label_id = LABEL_TO_ID["others"]
        else:
            events = load_events(csv_path)
            stats = score_events(events, predictions)
            label_id = LABEL_TO_ID[csv_path.parent.name]

        file_count += 1
        for key, value in stats.items():
            per_class[label_id][key] += value
            totals[key] += value

    lines = [
        "",
        "event-level evaluation on held-out files:",
        f"files: {file_count}",
        f"match tolerance ms: {EVENT_MATCH_TOLERANCE_MS}",
        f"confirm windows: {GESTURE_CONFIRM_WINDOWS}",
        f"cooldown ms: {GESTURE_OUTPUT_COOLDOWN_MS}",
        f"min rf votes: {GESTURE_RF_MIN_VOTES}/{len(clf.estimators_)}",
        "",
        "per class:",
    ]
    for label_id, stats in per_class.items():
        positives = stats["tp"] + stats["wrong"] + stats["fn"]
        recall = stats["tp"] / positives if positives else 0.0
        lines.append(
            f"  {CLASS_NAMES[label_id]}: "
            f"tp={stats['tp']} wrong={stats['wrong']} dup={stats['duplicate']} "
            f"fp={stats['fp']} fn={stats['fn']} recall={recall:.4f}"
        )

    positives = totals["tp"] + totals["wrong"] + totals["fn"]
    precision_den = totals["tp"] + totals["wrong"] + totals["fp"] + totals["duplicate"]
    recall = totals["tp"] / positives if positives else 0.0
    precision = totals["tp"] / precision_den if precision_den else 0.0
    lines.extend(
        [
            "",
            "event totals:",
            (
                f"  tp={totals['tp']} wrong={totals['wrong']} duplicate={totals['duplicate']} "
                f"fp={totals['fp']} fn={totals['fn']}"
            ),
            f"  event precision={precision:.4f}",
            f"  event recall={recall:.4f}",
        ]
    )
    return "\n".join(lines) + "\n"


def leaf_class(tree, node_id: int) -> int:
    counts = tree.value[node_id][0]
    return int(np.argmax(counts))


def export_model(clf: RandomForestClassifier) -> None:
    left: list[int] = []
    right: list[int] = []
    feature: list[int] = []
    threshold: list[int] = []
    value: list[int] = []
    offsets: list[int] = []

    for estimator in clf.estimators_:
        tree = estimator.tree_
        offset = len(left)
        offsets.append(offset)
        for node_id in range(tree.node_count):
            l_child = int(tree.children_left[node_id])
            r_child = int(tree.children_right[node_id])
            if l_child < 0:
                left.append(-1)
                right.append(-1)
                feature.append(-1)
                threshold.append(0)
                value.append(leaf_class(tree, node_id))
            else:
                left.append(offset + l_child)
                right.append(offset + r_child)
                feature.append(int(tree.feature[node_id]))
                threshold.append(math.floor(float(tree.threshold[node_id])))
                value.append(0)

    header = f"""/*
 * Generated by algorithm_sim/train_random_forest.py.
 */

#ifndef APP_GESTURE_RF_MODEL_H
#define APP_GESTURE_RF_MODEL_H

#include <stdint.h>
#include "gesture_algo.h"

#define GESTURE_RF_FEATURE_COUNT {len(FEATURE_NAMES)}
#define GESTURE_RF_TREE_COUNT {len(offsets)}
#define GESTURE_RF_NODE_COUNT {len(left)}

typedef struct GestureRfFeatures_st {{
    int32_t value[GESTURE_RF_FEATURE_COUNT];
}} GestureRfFeatures;

GestureType gesture_rf_predict(const GestureRfFeatures *features);
GestureType gesture_rf_predict_with_votes(const GestureRfFeatures *features, uint8_t *best_votes);

#endif
"""

    source = f"""/*
 * Generated by algorithm_sim/train_random_forest.py.
 */

#include "gesture_rf_model.h"

#include <stddef.h>
#include <stdint.h>

static const int16_t g_tree_offsets[GESTURE_RF_TREE_COUNT] = {{
{format_array(offsets, 8)}
}};

static const int16_t g_left[GESTURE_RF_NODE_COUNT] = {{
{format_array(left, 12)}
}};

static const int16_t g_right[GESTURE_RF_NODE_COUNT] = {{
{format_array(right, 12)}
}};

static const int8_t g_feature[GESTURE_RF_NODE_COUNT] = {{
{format_array(feature, 16)}
}};

static const int32_t g_threshold[GESTURE_RF_NODE_COUNT] = {{
{format_array(threshold, 8)}
}};

static const uint8_t g_value[GESTURE_RF_NODE_COUNT] = {{
{format_array(value, 16)}
}};

GestureType gesture_rf_predict_with_votes(const GestureRfFeatures *features, uint8_t *best_votes)
{{
    uint16_t tree_id;
    uint8_t votes[5] = {{0}};
    uint8_t best_class = 0;
    uint8_t best_vote_count = 0;

    if (!features) {{
        if (best_votes) {{
            *best_votes = 0;
        }}
        return GESTURE_NONE;
    }}

    for (tree_id = 0; tree_id < GESTURE_RF_TREE_COUNT; tree_id++) {{
        int16_t node = g_tree_offsets[tree_id];

        while (g_left[node] >= 0) {{
            int8_t feature_index = g_feature[node];
            if (features->value[(uint8_t)feature_index] <= g_threshold[node]) {{
                node = g_left[node];
            }} else {{
                node = g_right[node];
            }}
        }}

        votes[g_value[node]]++;
    }}

    for (tree_id = 0; tree_id < 5; tree_id++) {{
        if (votes[tree_id] > best_vote_count) {{
            best_vote_count = votes[tree_id];
            best_class = (uint8_t)tree_id;
        }}
    }}

    if (best_votes) {{
        *best_votes = best_vote_count;
    }}
    return (GestureType)best_class;
}}

GestureType gesture_rf_predict(const GestureRfFeatures *features)
{{
    uint8_t best_votes;

    return gesture_rf_predict_with_votes(features, &best_votes);
}}
"""

    (SDK_APP_DIR / "gesture_rf_model.h").write_text(header, newline="\n")
    (SDK_APP_DIR / "gesture_rf_model.c").write_text(source, newline="\n")


def format_array(values: list[int], per_line: int) -> str:
    lines = []
    for start in range(0, len(values), per_line):
        chunk = values[start : start + per_line]
        lines.append("    " + ", ".join(str(value) for value in chunk) + ",")
    return "\n".join(lines)


def main() -> None:
    windows = collect_labeled_windows()
    model, report, test_groups = train_model(windows)
    report += evaluate_event_level(model, test_groups)
    REPORT_PATH.write_text(report, newline="\n")
    export_model(model)
    print(report)
    print(f"wrote {REPORT_PATH}")
    print(f"wrote {SDK_APP_DIR / 'gesture_rf_model.c'}")
    print(f"wrote {SDK_APP_DIR / 'gesture_rf_model.h'}")


if __name__ == "__main__":
    main()
