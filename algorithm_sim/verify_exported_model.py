from __future__ import annotations

import re
from pathlib import Path

import joblib
import numpy as np

import optimize_gesture_model as experiment


MODEL_PATH = Path(__file__).resolve().parent / "gesture_rf_final.joblib"


def find_c_model_path() -> Path:
    script = Path(__file__).resolve()
    for base in (script.parents[1], script.parents[2]):
        for project_root in (
            base / "VeriHealthi_QEMU_SDK_v3.7",
            base / "01_完整工程" / "VeriHealthi_QEMU_SDK_v3.7",
        ):
            candidate = project_root / "galaxy_sdk" / "app" / "gesture_rf_model.c"
            if candidate.is_file():
                return candidate
    return script.parents[1] / "VeriHealthi_QEMU_SDK_v3.7" / "galaxy_sdk" / "app" / "gesture_rf_model.c"


C_MODEL_PATH = find_c_model_path()


def parse_array(source: str, name: str) -> np.ndarray:
    match = re.search(rf"{name}\[[^]]+\]\s*=\s*\{{(.*?)\}};", source, re.S)
    if not match:
        raise ValueError(f"array not found: {name}")
    return np.fromstring(match.group(1).strip().rstrip(","), dtype=np.int64, sep=",")


def parse_define(source: str, name: str) -> int:
    match = re.search(rf"#define\s+{name}\s+(-?\d+)", source)
    if not match:
        raise ValueError(f"define not found: {name}")
    return int(match.group(1))


def exported_votes(features: np.ndarray, arrays: dict[str, np.ndarray]) -> np.ndarray:
    votes = np.zeros((len(features), len(experiment.CLASS_NAMES)), dtype=np.int16)
    for row_index, row in enumerate(features):
        for offset in arrays["offsets"]:
            node = 0
            while arrays["right"][offset + node] != 0xFFFF:
                array_index = offset + node
                feature_index = arrays["feature"][array_index]
                if row[feature_index] + arrays["threshold_bias"] <= arrays["threshold"][array_index]:
                    node += 1
                else:
                    node = arrays["right"][array_index]
            votes[row_index, arrays["feature"][offset + node]] += 1
    return votes


def main() -> None:
    model = joblib.load(MODEL_PATH)
    source = C_MODEL_PATH.read_text(encoding="ascii")
    arrays = {
        "offsets": parse_array(source, "g_tree_offsets"),
        "right": parse_array(source, "g_right"),
        "feature": parse_array(source, "g_feature"),
        "threshold": parse_array(source, "g_threshold"),
        "threshold_bias": parse_define(source, "RF_THRESHOLD_BIAS"),
    }

    recordings = experiment.load_recordings()
    rng = np.random.default_rng(2026)
    vectors: list[np.ndarray] = []
    for recording in rng.choice(recordings, size=40, replace=False):
        if len(recording.samples) < 50:
            continue
        end = int(rng.integers(49, len(recording.samples)))
        window = recording.samples[end - 49 : end + 1]
        vectors.append(experiment.extract_features(window, rich=True, dynamic=True))
    features = np.asarray(vectors, dtype=np.int32)

    python_votes = experiment.predict_votes(model, features)
    c_votes = exported_votes(features, arrays)
    if not np.array_equal(python_votes, c_votes):
        mismatch = np.argwhere(python_votes != c_votes)[0]
        raise AssertionError(
            f"vote mismatch at vector={mismatch[0]} class={mismatch[1]}: "
            f"python={python_votes[tuple(mismatch)]} c={c_votes[tuple(mismatch)]}"
        )

    print(f"verified {len(features)} feature vectors")
    print(f"verified {len(model.estimators_)} trees and {len(arrays['right'])} nodes")
    print("Python and exported C forest votes are identical")


if __name__ == "__main__":
    main()
