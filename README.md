# VESC 2026 IMU Gesture Recognition

VESC 2026 VeriHealthi IMU 手势识别工程。项目基于 VeriHealthi QEMU SDK，在 NucleiStudio 中读取 IMU 数据，使用随机森林模型识别 `pinch`、`clench`、`up`、`down` 和 `others`。

## 目录说明

- `VeriHealthi_QEMU_SDK_v3.7/`：主工程 SDK，NucleiStudio 导入 `qemu` 工程后编译运行。
- `VeriHealthi_QEMU_SDK_v3.7/galaxy_sdk/app/`：比赛业务代码。
  - `imu_pipeline.c/.h`：IMU 初始化、采样、CRC32、滑动窗口发送。
  - `imu_sample.h`：IMU 样本和算法窗口结构定义。
  - `algo_task.c/.h`：算法任务，接收 IMU 窗口事件并输出识别结果。
  - `gesture_algo.c/.h`：特征提取、随机森林调用、输出确认和冷却控制。
  - `gesture_rf_model.c/.h`：由 Python 脚本导出的随机森林模型。
- `algorithm_sim/`：离线训练与评估脚本。
  - `train_random_forest.py`：读取数据集、训练随机森林、评估准确率、导出 C 模型。
  - `rf_training_report.txt`：最近一次训练和事件级评估结果。
- `VeriHealthi_IMU_Dataset/`：主办方 IMU 数据集，包含 `pinch`、`clench`、`up`、`down`、`others`。
- `docs/`：赛题、SDK 手册、算法开发手册。
- `submission/`：提交材料整理目录。

## 训练模型

```powershell
cd D:\A_RivenHuang\Electronic_Design_Project\VESC_2026
python .\algorithm_sim\train_random_forest.py
```

脚本会更新：

- `algorithm_sim/rf_training_report.txt`
- `VeriHealthi_QEMU_SDK_v3.7/galaxy_sdk/app/gesture_rf_model.c`
- `VeriHealthi_QEMU_SDK_v3.7/galaxy_sdk/app/gesture_rf_model.h`

## 编译运行

1. 打开 NucleiStudio。
2. 导入 `VeriHealthi_QEMU_SDK_v3.7/qemu` 工程。
3. 右键项目 `qemu`，执行 `Refresh`。
4. 执行 `Project -> Clean...`。
5. 编译并 Debug 运行。

运行后会打印系统初始化信息、识别到的手势事件，以及首次 320000 Byte IMU 数据的 CRC32。

