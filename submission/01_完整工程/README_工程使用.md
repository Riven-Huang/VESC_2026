# 工程使用说明

1. 打开 NucleiStudio IDE。
2. 选择 `File > Import > Existing Projects into Workspace`。
3. 导入本目录下 `VeriHealthi_QEMU_SDK_v3.7/qemu`。
4. 右键工程执行 `Clean Project`，让 IDE 按当前电脑路径重新生成构建文件。
5. 执行 `Build Project`，选择 `debug_qemu` 启动调试。

`qemu/Release` 中保留了本次验证的编译产物。生成的 Makefile 含本机绝对路径，换电脑后先 Clean 即可刷新。
