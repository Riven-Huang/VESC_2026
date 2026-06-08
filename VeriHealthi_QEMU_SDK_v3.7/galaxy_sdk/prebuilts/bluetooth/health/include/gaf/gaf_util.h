/** @file
 *  @brief Bluetooth subsystem logging helpers.
 */

/*
 * Copyright (C) 2025. VeriSilicon Holdings Co., Ltd. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define Z_FOR_LOOP_GET_ARG(_1, _2, _3, N, ...) N
#define Z_FOR_LOOP_0(z_call, sep, fixed_arg0, fixed_arg1, ...)
#define Z_FOR_LOOP_1(z_call, sep, fixed_arg0, fixed_arg1, x) \
	z_call(0, x, fixed_arg0, fixed_arg1)
#define Z_FOR_LOOP_2(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_1(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(1, x, fixed_arg0, fixed_arg1)
#define Z_FOR_LOOP_3(z_call, sep, fixed_arg0, fixed_arg1, x, ...) \
	Z_FOR_LOOP_2(z_call, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__) \
	__DEBRACKET sep \
	z_call(2, x, fixed_arg0, fixed_arg1)
#define Z_FOR_EACH_ENGINE(x, sep, fixed_arg0, fixed_arg1, ...) \
	Z_FOR_LOOP_GET_ARG(__VA_ARGS__, \
		Z_FOR_LOOP_3, \
		Z_FOR_LOOP_2, \
		Z_FOR_LOOP_1, \
		Z_FOR_LOOP_0)(x, sep, fixed_arg0, fixed_arg1, ##__VA_ARGS__)
#define Z_FOR_EACH_EXEC(idx, x, fixed_arg0, fixed_arg1) \
	fixed_arg0(x)
#define Z_FOR_EACH(F, sep, ...) \
	Z_FOR_EACH_ENGINE(Z_FOR_EACH_EXEC, sep, F, _, __VA_ARGS__)
#define Z_BYPASS(x) x
#define REVERSE_ARGS(...) \
	Z_FOR_EACH_ENGINE(Z_FOR_EACH_EXEC, (,), Z_BYPASS, _, __VA_ARGS__)
#define FOR_EACH(F, sep, ...) \
	Z_FOR_EACH(F, sep, REVERSE_ARGS(__VA_ARGS__))


/* Used to remove brackets from around a single argument. */
#define __DEBRACKET(...) __VA_ARGS__
#define __GET_ARG2_DEBRACKET(ignore_this, val, ...) __DEBRACKET val
#define __COND_CODE(one_or_two_args, _if_code, _else_code) \
	__GET_ARG2_DEBRACKET(one_or_two_args _if_code, _else_code)
#define Z_COND_CODE_1(_flag, _if_1_code, _else_code) \
	__COND_CODE(_XXXX##_flag, _if_1_code, _else_code)
/**
 * @brief Insert code depending on whether @p _flag expands to 1 or not.
 *
 * This relies on similar tricks as IS_ENABLED(), but as the result of
 * @p _flag expansion, results in either @p _if_1_code or @p
 * _else_code is expanded.
 *
 * To prevent the preprocessor from treating commas as argument
 * separators, the @p _if_1_code and @p _else_code expressions must be
 * inside brackets/parentheses: <tt>()</tt>. These are stripped away
 * during macro expansion.
 *
 * Example:
 *
 *     COND_CODE_1(CONFIG_FLAG, (uint32_t x;), (there_is_no_flag();))
 *
 * If @p CONFIG_FLAG is defined to 1, this expands to:
 *
 *     uint32_t x;
 *
 * It expands to <tt>there_is_no_flag();</tt> otherwise.
 *
 * This could be used as an alternative to:
 *
 *     #if defined(CONFIG_FLAG) && (CONFIG_FLAG == 1)
 *     #define MAYBE_DECLARE(x) uint32_t x
 *     #else
 *     #define MAYBE_DECLARE(x) there_is_no_flag()
 *     #endif
 *
 *     MAYBE_DECLARE(x);
 *
 * However, the advantage of COND_CODE_1() is that code is resolved in
 * place where it is used, while the @p \#if method defines @p
 * MAYBE_DECLARE on two lines and requires it to be invoked again on a
 * separate line. This makes COND_CODE_1() more concise and also
 * sometimes more useful when used within another macro's expansion.
 *
 * @note @p _flag can be the result of preprocessor expansion, e.g.
 *	 an expression involving <tt>NUM_VA_ARGS_LESS_1(...)</tt>.
 *	 However, @p _if_1_code is only expanded if @p _flag expands
 *	 to the integer literal 1. Integer expressions that evaluate
 *	 to 1, e.g. after doing some arithmetic, will not work.
 *
 * @param _flag evaluated flag
 * @param _if_1_code result if @p _flag expands to 1; must be in parentheses
 * @param _else_code result otherwise; must be in parentheses
 */
#define COND_CODE_1(_flag, _if_1_code, _else_code) \
	Z_COND_CODE_1(_flag, _if_1_code, _else_code)

