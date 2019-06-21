//
// CDDL HEADER START
//
// The contents of this file are subject to the terms of the
// Common Development and Distribution License, Version 1.0 only
// (the "License").  You may not use this file except in compliance
// with the License.
//
// You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
// or http://www.opensolaris.org/os/licensing.
// See the License for the specific language governing permissions
// and limitations under the License.
//
// When distributing Covered Code, include this CDDL HEADER in each
// file and include the License file at usr/src/OPENSOLARIS.LICENSE.
// If applicable, add the following below this CDDL HEADER, with the
// fields enclosed by brackets "[]" replaced with your own identifying
// information: Portions Copyright [yyyy] [name of copyright owner]
//
// CDDL HEADER END
//
//
// Copyright (c) 1993-1997, by Sun Microsystems, Inc.
// All rights reserved.
//
      .text
      .align  16
      .globl  get_cpuid
      .type   get_cpuid, @function
get_cpuid:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	movl	8(%ebp), %eax
	cpuid
	pushl	%eax
	movl	0x0c(%ebp), %eax
	movl	%ebx, (%eax)
	movl	0x10(%ebp), %eax
	movl	%ecx, (%eax)
	movl	0x14(%ebp), %eax
	movl	%edx, (%eax)
	popl	%eax
	popl	%ebx
	popl	%ebp
	ret

      .text
      .align  16
      .globl  get_cpuid4
      .type   get_cpuid4, @function
get_cpuid4:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	movl	$4, %eax
	movl	$0, %ecx
	cpuid
	pushl	%eax
	movl	0x08(%ebp), %eax
	movl	%ebx, (%eax)
	movl	0x0c(%ebp), %eax
	movl	%ecx, (%eax)
	movl	0x10(%ebp), %eax
	movl	%edx, (%eax)
	popl	%eax
	popl	%ebx
	popl	%ebp
	ret

