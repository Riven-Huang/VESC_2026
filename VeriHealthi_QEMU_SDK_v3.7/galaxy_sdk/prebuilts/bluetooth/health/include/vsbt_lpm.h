/**
 * Copyright (C) 2025. VeriSilicon Holdings Co., Ltd.
 * All rights reserved.
 *
 * @file vsbt_lpm.h
 * @brief public head file of vsbt lpm
 * @author Hongliang Mao <Hongliang.Mao@verisilicon.com>
 */
#ifndef _VSBT_LPM_H_
#define _VSBT_LPM_H_

void vsbt_lpm_enable();
void vsbt_lpm_disable();

int vsbt_lpm_save();
int vsbt_lpm_restore();
int vsbt_lpm_judge();

#endif
