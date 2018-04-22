	.syntax	unified
	.arch	armv7-m

/* sections in this file make up the header page of the kernel module */
  .section .binary_layout
  .globl __binary_layout
__binary_layout:
  .long	__text_start__
  .long __text_end__
  .long __got_start__
  .long __got_end__
	.long	__data_start__
	.long	__data_end__
  .long __bss_start__
  .long __bss_end__
  .long 0x400       /* HEAP SIZE */
  .size __binary_layout, . - __binary_layout

  .section .jump_vector
  .align	2
  .globl	__jump_vector
__jump_vector:

	/* jump vectors. replace Default_Handler with you handler as required*/
  .long moduleInit      // 0
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler // 10

  .long Default_Handler // 11
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler // 20

  .long Default_Handler // 21
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler // 30

  .long Default_Handler // 31
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler
  .long Default_Handler // 40

	.size	__jump_vector, . - __jump_vector

	.section .text
	.thumb
	.align	1
	.thumb_func
  .globl Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler


/* thanks Matthew Slattery for this little gem */
  .macro padded_string string, max
1:
   .ascii "\string"
2:
   .iflt \max - (2b - 1b)
   .error "String too long"
   .endif

   .ifgt \max - (2b - 1b)
   .zero \max - (2b - 1b)
   .endif

   .endm


  .section .module_name
__module_name:
  padded_string "example module", 60 @ module name
  .long 0x00000001 @ module ID
