from __future__ import annotations

import argparse
import csv
import re
import time
from dataclasses import dataclass
from pathlib import Path

import numpy as np
from sklearn.ensemble import ExtraTreesClassifier, RandomForestClassifier
from sklearn.metrics import classification_report, confusion_matrix


def find_dataset_dir() -> Path:
    script = Path(__file__).resolve()
    for base in (script.parents[1], script.parents[2]):
        candidate = base / "VeriHealthi_IMU_Dataset"
        if candidate.is_dir():
            return candidate
    return script.parents[1] / "VeriHealthi_IMU_Dataset"


DATASET_DIR = find_dataset_dir()

SAMPLE_RATE_HZ = 50
WINDOW_STEP_SAMPLES = 10
POSITIVE_END_OFFSETS = range(-10, 31, 5)
NEGATIVE_STEP_SAMPLES = 25
NEGATIVE_MARGIN_SAMPLES = 75

LABEL_TO_ID = {
    "others": 0,
    "pinch": 1,
    "clench": 2,
    "up": 3,
    "down": 4,
}
CLASS_NAMES = ["others", "pinch", "clench", "up", "down"]
OUTPUT_LABELS = (1, 2, 3, 4)


@dataclass(frozen=True)
class Event:
    sample_index: int
    label_id: int

    @property
    def timestamp_ms(self) -> int:
        return self.sample_index * 1000 // SAMPLE_RATE_HZ


@dataclass
class Recording:
    key: str
    user_id: str
    samples: np.ndarray
    events: list[Event]


@dataclass(frozen=True)
class PostprocessConfig:
    min_votes: tuple[int, int, int, int, int]
    confirm_windows: tuple[int, int, int, int, int]
    cooldown_ms: int
    release_windows: int
    use_arm_state: bool


@dataclass
class EventScore:
    tp: np.ndarray
    fp: np.ndarray
    fn: np.ndarray
    latencies_ms: list[int]
    errors: np.ndarray
    predictions: int = 0
    duration_ms: int = 0

    @classmethod
    def empty(cls) -> EventScore:
        return cls(
            tp=np.zeros(len(CLASS_NAMES), dtype=np.int64),
            fp=np.zeros(len(CLASS_NAMES), dtype=np.int64),
            fn=np.zeros(len(CLASS_NAMES), dtype=np.int64),
            latencies_ms=[],
            errors=np.zeros((len(CLASS_NAMES), len(CLASS_NAMES)), dtype=np.int64),
        )

    def add(self, other: EventScore) -> None:
        self.tp += other.tp
        self.fp += other.fp
        self.fn += other.fn
        self.latencies_ms.extend(other.latencies_ms)
        self.errors += other.errors
        self.predictions += other.predictions
        self.duration_ms += other.duration_ms


@dataclass
class OneVsRestForest:
    estimators: dict[int, RandomForestClassifier]


def load_raw_samples(path: Path) -> np.ndarray:
    values: list[int] = []
    for line in path.read_text(errors="ignore").splitlines()[5:]:
        try:
            values.append(int(line.strip()))
        except ValueError:
            continue

    frame_count = len(values) // 7
    if frame_count == 0:
        return np.empty((0, 6), dtype=np.int32)
    frames = np.asarray(values[: frame_count * 7], dtype=np.int64).reshape(frame_count, 7)
    return frames[:, :6].astype(np.int32)


def load_csv_events(path: Path, label_id: int) -> list[Event]:
    events: list[Event] = []
    with path.open(newline="") as handle:
        for row in csv.DictReader(handle):
            sample_index = int(round(float(row["event_time_s"]) * SAMPLE_RATE_HZ))
            events.append(Event(sample_index, label_id))
    return events


def user_id_from_name(name: str) -> str:
    match = re.search(r"_ID(\d+)", name)
    if not match:
        raise ValueError(f"missing user ID in {name}")
    return match.group(1)


def matching_csv(txt_path: Path, label: str) -> Path:
    stem = txt_path.stem
    if stem.startswith("IMU_"):
        stem = stem[4:]
    candidate = DATASET_DIR / label / f"{stem}.csv"
    if not candidate.exists():
        raise FileNotFoundError(candidate)
    return candidate


def load_recordings() -> list[Recording]:
    recordings: list[Recording] = []

    for label in ("pinch", "clench"):
        label_id = LABEL_TO_ID[label]
        for txt_path in sorted((DATASET_DIR / label).glob("*.txt")):
            events = load_csv_events(matching_csv(txt_path, label), label_id)
            recordings.append(
                Recording(
                    key=txt_path.stem,
                    user_id=user_id_from_name(txt_path.stem),
                    samples=load_raw_samples(txt_path),
                    events=events,
                )
            )

    # The up and down directories contain duplicate copies of each up/down recording.
    for txt_path in sorted((DATASET_DIR / "up").glob("*.txt")):
        events = load_csv_events(matching_csv(txt_path, "up"), LABEL_TO_ID["up"])
        events.extend(load_csv_events(matching_csv(txt_path, "down"), LABEL_TO_ID["down"]))
        events.sort(key=lambda event: event.sample_index)
        recordings.append(
            Recording(
                key=txt_path.stem,
                user_id=user_id_from_name(txt_path.stem),
                samples=load_raw_samples(txt_path),
                events=events,
            )
        )

    for txt_path in sorted((DATASET_DIR / "others").glob("*.txt")):
        recordings.append(
            Recording(
                key=txt_path.stem,
                user_id=user_id_from_name(txt_path.stem),
                samples=load_raw_samples(txt_path),
                events=[],
            )
        )

    return recordings


def rounded_mean(values: np.ndarray, axis: int = 0) -> np.ndarray:
    total = values.astype(np.int64).sum(axis=axis)
    count = values.shape[axis]
    rounded = np.where(
        total >= 0,
        (total + count // 2) // count,
        -((-total + count // 2) // count),
    )
    return rounded.astype(np.int32)


def extract_features(window: np.ndarray, rich: bool, dynamic: bool) -> np.ndarray:
    axis_min = window.min(axis=0)
    axis_max = window.max(axis=0)
    axis_range = axis_max - axis_min
    axis_mean = rounded_mean(window)
    delta = window[-1] - window[0]
    gyro_abs_mean = int(np.rint(np.abs(window[:, :3]).sum(axis=1).mean()))
    accel_abs_mean = int(np.rint(np.abs(window[:, 3:6]).sum(axis=1).mean()))

    if dynamic:
        centered = window - axis_mean
        gyro_abs_mean = int(rounded_mean(np.abs(centered[:, :3]).sum(axis=1)))
        accel_abs_mean = int(rounded_mean(np.abs(centered[:, 3:6]).sum(axis=1)))
        features: list[int] = [
            *axis_range.tolist(),
            *axis_mean[3:6].tolist(),
            *delta.tolist(),
            gyro_abs_mean,
            accel_abs_mean,
            int(axis_range[:3].sum()),
            int(axis_range[3:6].sum()),
        ]
    else:
        centered = window - axis_mean
        features = [
            *axis_range.tolist(),
            *axis_mean.tolist(),
            *delta.tolist(),
            gyro_abs_mean,
            accel_abs_mean,
            int(axis_range[:3].sum()),
            int(axis_range[3:6].sum()),
        ]
    if not rich:
        return np.asarray(features, dtype=np.int32)

    mean_abs_deviation = rounded_mean(np.abs(window - axis_mean))
    half = len(window) // 2
    half_delta = rounded_mean(window[half:]) - rounded_mean(window[:half])
    quarter = max(1, len(window) // 4)
    quarter_delta = rounded_mean(window[-quarter:]) - rounded_mean(window[:quarter])
    max_abs = np.abs(centered if dynamic else window).max(axis=0)

    features.extend(mean_abs_deviation.tolist())
    features.extend(half_delta.tolist())
    features.extend(quarter_delta.tolist())
    features.extend(max_abs.tolist())

    for segment in np.array_split(window, 5):
        segment_mean = rounded_mean(segment)
        if dynamic:
            segment_mean = segment_mean - axis_mean
        features.extend(segment_mean.tolist())
    for segment in np.array_split(window, 5):
        features.extend((segment.max(axis=0) - segment.min(axis=0)).tolist())

    if dynamic:
        gyro_activity = np.abs(centered[:, :3]).sum(axis=1)
        accel_activity = np.abs(centered[:, 3:6]).sum(axis=1)
        for gyro_segment, accel_segment in zip(
            np.array_split(gyro_activity, 5), np.array_split(accel_activity, 5)
        ):
            features.extend(
                [
                    int(rounded_mean(gyro_segment)),
                    int(gyro_segment.max()),
                    int(rounded_mean(accel_segment)),
                    int(accel_segment.max()),
                ]
            )
    return np.asarray(features, dtype=np.int32)


def window_at(samples: np.ndarray, end_index: int, window_samples: int) -> np.ndarray | None:
    start = end_index + 1 - window_samples
    if start < 0 or end_index >= len(samples):
        return None
    return samples[start : end_index + 1]


def is_far_from_events(index: int, events: list[Event]) -> bool:
    return all(abs(index - event.sample_index) >= NEGATIVE_MARGIN_SAMPLES for event in events)


def build_training_set(
    recordings: list[Recording],
    window_samples: int,
    rich: bool,
    dynamic: bool,
    augment: bool = False,
) -> tuple[np.ndarray, np.ndarray]:
    features: list[np.ndarray] = []
    labels: list[int] = []

    for recording in recordings:
        for event in recording.events:
            for offset in POSITIVE_END_OFFSETS:
                window = window_at(recording.samples, event.sample_index + offset, window_samples)
                if window is not None:
                    features.append(extract_features(window, rich, dynamic))
                    labels.append(event.label_id)
                    if augment:
                        center = rounded_mean(window).astype(np.int64)
                        centered = window.astype(np.int64) - center
                        for numerator in (4, 6):
                            scaled = center + np.rint(centered * numerator / 5.0).astype(np.int64)
                            features.append(
                                extract_features(scaled.astype(np.int32), rich, dynamic)
                            )
                            labels.append(event.label_id)

        first_end = window_samples - 1
        for end_index in range(first_end, len(recording.samples), NEGATIVE_STEP_SAMPLES):
            if is_far_from_events(end_index, recording.events):
                window = window_at(recording.samples, end_index, window_samples)
                if window is not None:
                    features.append(extract_features(window, rich, dynamic))
                    labels.append(LABEL_TO_ID["others"])

    return np.asarray(features, dtype=np.int32), np.asarray(labels, dtype=np.int32)


def build_stream_features(
    recording: Recording, window_samples: int, rich: bool, dynamic: bool
) -> tuple[np.ndarray, np.ndarray]:
    features: list[np.ndarray] = []
    timestamps_ms: list[int] = []
    for end_index in range(window_samples - 1, len(recording.samples), WINDOW_STEP_SAMPLES):
        window = window_at(recording.samples, end_index, window_samples)
        if window is not None:
            features.append(extract_features(window, rich, dynamic))
            timestamps_ms.append(end_index * 1000 // SAMPLE_RATE_HZ)
    return np.asarray(features, dtype=np.int32), np.asarray(timestamps_ms, dtype=np.int32)


def make_model(kind: str, trees: int, depth: int, leaf: int, seed: int, jobs: int):
    common = dict(
        n_estimators=trees,
        max_depth=depth,
        min_samples_leaf=leaf,
        class_weight="balanced_subsample" if kind == "rf" else "balanced",
        random_state=seed,
        n_jobs=jobs,
    )
    if kind == "rf":
        return RandomForestClassifier(max_features="sqrt", **common)
    if kind == "extra":
        return ExtraTreesClassifier(max_features="sqrt", **common)
    if kind == "ovr":
        return OneVsRestForest(
            estimators={
                label_id: RandomForestClassifier(max_features="sqrt", **common)
                for label_id in OUTPUT_LABELS
            }
        )
    raise ValueError(kind)


def fit_model(model, features: np.ndarray, labels: np.ndarray) -> None:
    if isinstance(model, OneVsRestForest):
        for label_id, estimator in model.estimators.items():
            estimator.fit(features, labels == label_id)
        return
    model.fit(features, labels)


def predict_votes(model, features: np.ndarray) -> np.ndarray:
    votes = np.zeros((len(features), len(CLASS_NAMES)), dtype=np.int16)
    row_indices = np.arange(len(features))
    if isinstance(model, OneVsRestForest):
        for label_id, forest in model.estimators.items():
            for estimator in forest.estimators_:
                labels = estimator.predict(features).astype(np.int32)
                votes[:, label_id] += labels
        return votes

    for estimator in model.estimators_:
        labels = estimator.predict(features).astype(np.int32)
        votes[row_indices, labels] += 1
    return votes


def postprocess_predictions(
    timestamps_ms: np.ndarray,
    votes: np.ndarray,
    config: PostprocessConfig,
) -> list[tuple[int, int]]:
    predictions: list[tuple[int, int]] = []
    pending_label = 0
    pending_count = 0
    last_output_ms = -config.cooldown_ms
    arm_raised = False
    blocked_label = 0
    release_count = 0

    for timestamp_ms, row in zip(timestamps_ms.tolist(), votes):
        label_id = int(np.argmax(row))
        if label_id == 0 or int(row[label_id]) < config.min_votes[label_id]:
            label_id = 0

        if blocked_label != 0:
            if label_id == blocked_label:
                release_count = 0
                continue
            release_count += 1
            if release_count < config.release_windows:
                continue
            blocked_label = 0
            release_count = 0

        if label_id == 0:
            pending_label = 0
            pending_count = 0
            continue

        if config.use_arm_state:
            if label_id == LABEL_TO_ID["up"] and arm_raised:
                continue
            if label_id == LABEL_TO_ID["down"] and not arm_raised:
                continue

        if label_id == pending_label:
            pending_count += 1
        else:
            pending_label = label_id
            pending_count = 1

        if pending_count < config.confirm_windows[label_id]:
            continue
        if timestamp_ms - last_output_ms < config.cooldown_ms:
            continue

        predictions.append((timestamp_ms, label_id))
        last_output_ms = timestamp_ms
        pending_label = 0
        pending_count = 0
        blocked_label = label_id
        release_count = 0
        if label_id == LABEL_TO_ID["up"]:
            arm_raised = True
        elif label_id == LABEL_TO_ID["down"]:
            arm_raised = False

    return predictions


def postprocess_pooled_predictions(
    timestamps_ms: np.ndarray,
    votes: np.ndarray,
    config: PostprocessConfig,
    decision_windows: int = 4,
    release_windows: int = 2,
) -> list[tuple[int, int]]:
    predictions: list[tuple[int, int]] = []
    index = 0
    last_output_ms = -config.cooldown_ms
    frame_count = len(timestamps_ms)

    def accepted_label(row: np.ndarray) -> int:
        label_id = int(np.argmax(row))
        if label_id == 0 or int(row[label_id]) < config.min_votes[label_id]:
            return 0
        return label_id

    while index < frame_count:
        if accepted_label(votes[index]) == 0:
            index += 1
            continue

        end = min(frame_count, index + decision_windows)
        scores = np.zeros(len(CLASS_NAMES), dtype=np.int32)
        support = np.zeros(len(CLASS_NAMES), dtype=np.int16)
        for row in votes[index:end]:
            for label_id in OUTPUT_LABELS:
                threshold = config.min_votes[label_id]
                if int(row[label_id]) >= threshold:
                    scores[label_id] += int(row[label_id]) * 256 // threshold
                    support[label_id] += 1

        label_id = int(np.argmax(scores))
        output_ms = int(timestamps_ms[end - 1])
        if support[label_id] >= config.confirm_windows[label_id]:
            if output_ms - last_output_ms >= config.cooldown_ms:
                predictions.append((output_ms, label_id))
                last_output_ms = output_ms

        index = end
        inactive_count = 0
        while index < frame_count and inactive_count < release_windows:
            if accepted_label(votes[index]) == 0:
                inactive_count += 1
            else:
                inactive_count = 0
            index += 1

    return predictions


def postprocess_peak_predictions(
    timestamps_ms: np.ndarray,
    votes: np.ndarray,
    config: PostprocessConfig,
    lookahead_windows: int = 4,
) -> list[tuple[int, int]]:
    predictions: list[tuple[int, int]] = []
    index = 0
    last_output_ms = -config.cooldown_ms
    frame_count = len(timestamps_ms)

    while index < frame_count:
        row = votes[index]
        candidates = [
            label_id
            for label_id in OUTPUT_LABELS
            if int(row[label_id]) >= config.min_votes[label_id]
        ]
        if not candidates:
            index += 1
            continue

        end = min(frame_count, index + lookahead_windows)
        best_label = 0
        best_votes = -1
        for candidate_row in votes[index:end]:
            for label_id in OUTPUT_LABELS:
                vote_count = int(candidate_row[label_id])
                if vote_count >= config.min_votes[label_id] and vote_count > best_votes:
                    best_votes = vote_count
                    best_label = label_id

        output_ms = int(timestamps_ms[end - 1])
        if best_label != 0 and output_ms - last_output_ms >= config.cooldown_ms:
            predictions.append((output_ms, best_label))
            last_output_ms = output_ms
        index = end

    return predictions


def score_recording(recording: Recording, predictions: list[tuple[int, int]]) -> EventScore:
    score = EventScore.empty()
    score.predictions = len(predictions)
    score.duration_ms = len(recording.samples) * 1000 // SAMPLE_RATE_HZ
    used_predictions: set[int] = set()

    for event_index, event in enumerate(recording.events):
        start_ms = event.timestamp_ms - 300
        if event_index + 1 < len(recording.events):
            end_ms = recording.events[event_index + 1].timestamp_ms - 300
        else:
            end_ms = score.duration_ms

        candidates = [
            index
            for index, (timestamp_ms, _) in enumerate(predictions)
            if start_ms <= timestamp_ms < end_ms
        ]
        correct = next(
            (index for index in candidates if predictions[index][1] == event.label_id), None
        )
        if correct is None:
            score.fn[event.label_id] += 1
        else:
            score.tp[event.label_id] += 1
            score.latencies_ms.append(predictions[correct][0] - event.timestamp_ms)
            used_predictions.add(correct)

        for index in candidates:
            if index != correct:
                predicted_label = predictions[index][1]
                score.fp[predicted_label] += 1
                score.errors[event.label_id, predicted_label] += 1
                used_predictions.add(index)

    for index, (_, label_id) in enumerate(predictions):
        if index not in used_predictions:
            score.fp[label_id] += 1
            score.errors[LABEL_TO_ID["others"], label_id] += 1

    return score


def summarize_score(score: EventScore) -> dict[str, float]:
    f1_values: list[float] = []
    output: dict[str, float] = {}
    for label_id in OUTPUT_LABELS:
        tp = int(score.tp[label_id])
        fp = int(score.fp[label_id])
        fn = int(score.fn[label_id])
        precision = tp / (tp + fp) if tp + fp else 0.0
        recall = tp / (tp + fn) if tp + fn else 0.0
        f1 = 2 * precision * recall / (precision + recall) if precision + recall else 0.0
        output[f"{CLASS_NAMES[label_id]}_precision"] = precision
        output[f"{CLASS_NAMES[label_id]}_recall"] = recall
        output[f"{CLASS_NAMES[label_id]}_f1"] = f1
        f1_values.append(f1)

    output["macro_f1"] = float(np.mean(f1_values))
    output["fp_per_minute"] = (
        float(score.fp[1:].sum()) * 60000.0 / score.duration_ms if score.duration_ms else 0.0
    )
    output["mean_latency_ms"] = (
        float(np.mean(score.latencies_ms)) if score.latencies_ms else 0.0
    )
    return output


def print_score(title: str, score: EventScore) -> None:
    metrics = summarize_score(score)
    print(f"\n{title}")
    for label_id in OUTPUT_LABELS:
        name = CLASS_NAMES[label_id]
        print(
            f"  {name:7s} tp={score.tp[label_id]:4d} fp={score.fp[label_id]:4d} "
            f"fn={score.fn[label_id]:4d} precision={metrics[name + '_precision']:.4f} "
            f"recall={metrics[name + '_recall']:.4f} f1={metrics[name + '_f1']:.4f}"
        )
    print(
        f"  macro_f1={metrics['macro_f1']:.4f} "
        f"fp/min={metrics['fp_per_minute']:.3f} "
        f"mean_latency_ms={metrics['mean_latency_ms']:.1f}"
    )
    print("  error rows=true-window cols=predicted [pinch clench up down]")
    for true_label in range(len(CLASS_NAMES)):
        values = " ".join(str(int(score.errors[true_label, pred])) for pred in OUTPUT_LABELS)
        print(f"    {CLASS_NAMES[true_label]:7s} {values}")


def default_configs(tree_count: int) -> list[PostprocessConfig]:
    configs: list[PostprocessConfig] = []
    for pinch_fraction in (0.56, 0.64, 0.72, 0.80):
        pinch_votes = int(round(tree_count * pinch_fraction))
        for clench_fraction in (0.44, 0.52, 0.60, 0.68):
            clench_votes = int(round(tree_count * clench_fraction))
            for arm_fraction in (0.52, 0.60, 0.68):
                arm_votes = int(round(tree_count * arm_fraction))
                thresholds = (0, pinch_votes, clench_votes, arm_votes, arm_votes)
                for pinch_confirm in (2, 3):
                    for clench_confirm in (1, 2, 3):
                        for arm_confirm in (2, 3):
                            confirmations = (
                                0,
                                pinch_confirm,
                                clench_confirm,
                                arm_confirm,
                                arm_confirm,
                            )
                            for cooldown_ms in (600, 1000):
                                for use_arm_state in (False, True):
                                    configs.append(
                                        PostprocessConfig(
                                            min_votes=thresholds,
                                            confirm_windows=confirmations,
                                            cooldown_ms=cooldown_ms,
                                            release_windows=1,
                                            use_arm_state=use_arm_state,
                                        )
                                    )
    return configs


def evaluate_configs(
    recordings: list[Recording],
    stream_predictions: dict[str, tuple[np.ndarray, np.ndarray]],
    configs: list[PostprocessConfig],
    pooled: bool,
    peak: bool,
) -> tuple[PostprocessConfig, EventScore]:
    best_config = configs[0]
    best_score = EventScore.empty()
    best_metric = -1.0
    for config in configs:
        total = EventScore.empty()
        for recording in recordings:
            timestamps_ms, votes = stream_predictions[recording.key]
            if peak:
                predictions = postprocess_peak_predictions(timestamps_ms, votes, config)
            elif pooled:
                predictions = postprocess_pooled_predictions(timestamps_ms, votes, config)
            else:
                predictions = postprocess_predictions(timestamps_ms, votes, config)
            total.add(score_recording(recording, predictions))
        metric = summarize_score(total)["macro_f1"]
        if metric > best_metric:
            best_metric = metric
            best_config = config
            best_score = total
    return best_config, best_score


def run_experiment(args: argparse.Namespace) -> None:
    started = time.perf_counter()
    recordings = load_recordings()
    train_recordings = [r for r in recordings if r.user_id != args.validation_user]
    validation_recordings = [r for r in recordings if r.user_id == args.validation_user]
    print(
        f"recordings={len(recordings)} train={len(train_recordings)} "
        f"validation={len(validation_recordings)} user={args.validation_user}"
    )

    x_train, y_train = build_training_set(
        train_recordings, args.window, args.rich, args.dynamic, args.augment
    )
    x_valid, y_valid = build_training_set(
        validation_recordings, args.window, args.rich, args.dynamic
    )
    print(
        f"features={x_train.shape[1]} train_windows={len(x_train)} "
        f"validation_windows={len(x_valid)}"
    )

    model = make_model(args.model, args.trees, args.depth, args.leaf, args.seed, args.jobs)
    fit_model(model, x_train, y_train)
    if isinstance(model, OneVsRestForest):
        window_votes = predict_votes(model, x_valid)
        window_predictions = window_votes.argmax(axis=1)
        best_window_votes = window_votes.max(axis=1)
        window_predictions[best_window_votes < args.trees // 2] = 0
    else:
        window_predictions = model.predict(x_valid)
    print("\nwindow-level validation")
    print(classification_report(y_valid, window_predictions, target_names=CLASS_NAMES, digits=4))
    print(confusion_matrix(y_valid, window_predictions, labels=range(len(CLASS_NAMES))))

    stream_predictions: dict[str, tuple[np.ndarray, np.ndarray]] = {}
    for index, recording in enumerate(validation_recordings, start=1):
        features, timestamps_ms = build_stream_features(
            recording, args.window, args.rich, args.dynamic
        )
        stream_predictions[recording.key] = (timestamps_ms, predict_votes(model, features))
        if index % 10 == 0:
            print(f"stream inference {index}/{len(validation_recordings)}")

    if args.fixed_config:
        pinch_votes = args.pinch_votes or int(round(args.trees * 0.80))
        clench_votes = args.clench_votes or int(round(args.trees * 0.44))
        arm_votes = args.arm_votes or int(round(args.trees * 0.60))
        configs = [
            PostprocessConfig(
                min_votes=(
                    0,
                    pinch_votes,
                    clench_votes,
                    arm_votes,
                    arm_votes,
                ),
                confirm_windows=(
                    0,
                    args.pinch_confirm,
                    args.clench_confirm,
                    args.arm_confirm,
                    args.arm_confirm,
                ),
                cooldown_ms=args.cooldown,
                release_windows=args.release_windows,
                use_arm_state=args.arm_state,
            )
        ]
    else:
        configs = default_configs(args.trees)
    best_config, best_score = evaluate_configs(
        validation_recordings, stream_predictions, configs, args.pooled, args.peak
    )
    print(f"\nbest postprocess: {best_config}")
    print_score("official-window event validation", best_score)
    print(f"elapsed={time.perf_counter() - started:.1f}s")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Leakage-free IMU gesture model experiments")
    parser.add_argument("--model", choices=("rf", "extra", "ovr"), default="extra")
    parser.add_argument("--trees", type=int, default=50)
    parser.add_argument("--depth", type=int, default=10)
    parser.add_argument("--leaf", type=int, default=6)
    parser.add_argument("--window", type=int, default=50)
    parser.add_argument("--validation-user", default="5")
    parser.add_argument("--seed", type=int, default=2026)
    parser.add_argument("--jobs", type=int, default=-1)
    parser.add_argument("--rich", action="store_true")
    parser.add_argument("--dynamic", action="store_true")
    parser.add_argument("--fixed-config", action="store_true")
    parser.add_argument("--augment", action="store_true")
    parser.add_argument("--pooled", action="store_true")
    parser.add_argument("--peak", action="store_true")
    parser.add_argument("--pinch-votes", type=int)
    parser.add_argument("--clench-votes", type=int)
    parser.add_argument("--arm-votes", type=int)
    parser.add_argument("--pinch-confirm", type=int, default=3)
    parser.add_argument("--clench-confirm", type=int, default=3)
    parser.add_argument("--arm-confirm", type=int, default=3)
    parser.add_argument("--cooldown", type=int, default=1000)
    parser.add_argument("--release-windows", type=int, default=2)
    parser.add_argument("--arm-state", action="store_true")
    return parser.parse_args()


if __name__ == "__main__":
    run_experiment(parse_args())
