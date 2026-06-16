# 算法仿真说明

## 文件

- `optimize_gesture_model.py`：读取主办方数据集，提取滑窗特征，执行按用户留一验证和事件级评测。
- `train_final_model.py`：使用全部公开数据训练最终随机森林，并导出嵌入式 C 模型。
- `verify_exported_model.py`：比较 Python 模型与 C 数组的逐树投票结果。
- `gesture_rf_final.json`：模型配置、特征和导出信息。
- `gesture_rf_final.joblib`：Python 随机森林模型。
- `requirements.txt`：Python 依赖。

## 环境

建议使用 Python 3.10 或更新版本：

```powershell
python -m pip install -r requirements.txt
```

脚本会在当前提交目录及其上一级查找 `VeriHealthi_IMU_Dataset`，并将 C 模型写入 `01_完整工程/VeriHealthi_QEMU_SDK_v3.7/galaxy_sdk/app`。数据集由主办方提供，未重复放入提交包。

## 复现

在仓库根目录运行：

```powershell
python submission/02_算法仿真/optimize_gesture_model.py --model rf --trees 100 --depth 14 --leaf 2 --window 50 --validation-user 0 --rich --dynamic --fixed-config --pinch-votes 82 --clench-votes 77 --arm-votes 50 --cooldown 600 --release-windows 2 --arm-state
python submission/02_算法仿真/train_final_model.py
python submission/02_算法仿真/verify_exported_model.py
```

完整六用户留一结果见 `../03_设计文档/验证报告.md`。最终事件级宏 F1 为 `93.30%`。
