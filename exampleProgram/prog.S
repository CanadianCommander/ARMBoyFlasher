	.syntax	unified
	.arch	armv7-m

  .section .jump_vector
  .align	2
  .globl	__jump_vector
__jump_vector:

	/* entry point */
  .long main      // 0
	.size	__jump_vector, . - __jump_vector
