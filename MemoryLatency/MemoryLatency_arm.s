.text

.global latencytest
.global preplatencyarr
.global stlftest
.global stlftest32
.global matchedstlftest

/* x0 = ptr to arr
   x1 = arr len
   convert values in array from array indexes to pointers */
preplatencyarr:
  sub sp, sp, #0x20
  stp x14, x15, [sp, #0x10]
  mov x15, 0
preplatencyarr_loop:
  ldr x14, [x0, w15, uxtw #3]
  lsl x14, x14, 3
  add x14, x14, x0
  str x14, [x0, w15, uxtw #3]
  add w15, w15, 1
  cmp x15, x1
  b.ne preplatencyarr_loop
  ldp x14, x15, [sp, #0x10]
  add sp, sp, #0x20
  ret

/* x0 = iteration count
   x1 = ptr to arr
   do pointer chasing for specified iteration count */
latencytest:
  sub sp, sp, #0x20
  stp x14, x15, [sp, #0x10]
  mov x14, 0
  ldr x15, [x1]
latencytest_loop:
  ldr x15, [x15]
  add x14, x14, x15
  sub x0, x0, 1
  cbnz x0, latencytest_loop
  mov x0, x14
  ldp x14, x15, [sp, #0x10]
  add sp, sp, #0x20
  ret

/* x0 = iteration count
   x1 = ptr to arr. first 32-bit int = store offset, second = load offset */
stlftest:
  sub sp, sp, #0x40
  stp x14, x15, [sp, #0x10]
  stp x12, x13, [sp, #0x20]  /* x12 = store ptr, x13 = load ptr */
  ldr x15, [x1]
  ldr w12, [x1]
  ldr w13, [x1, 4]
  add x12, x12, x1
  add x13, x13, x1
stlftest_loop:
  str x15, [x12]
  ldr w15, [x13]
  str x15, [x12]
  ldr w15, [x13]
  str x15, [x12]
  ldr w15, [x13]
  str x15, [x12]
  ldr w15, [x13]
  str x15, [x12]
  ldr w15, [x13]
  sub x0, x0, 5
  cmp x0, 0
  b.gt stlftest_loop
  ldp x12, x13, [sp, #0x10]
  ldp x14, x15, [sp, #0x10]
  add sp, sp, #0x40
  ret

stlftest32:
  sub sp, sp, #0x40
  stp x14, x15, [sp, #0x10]
  stp x12, x13, [sp, #0x20]  /* x12 = store ptr, x13 = load ptr */
  ldr x15, [x1]
  ldr w12, [x1]
  ldr w13, [x1, 4]
  add x12, x12, x1
  add x13, x13, x1
stlftest32_loop:
  str w15, [x12]
  ldrh w15, [x13]
  str w15, [x12]
  ldrh w15, [x13]
  str w15, [x12]
  ldrh w15, [x13]
  str w15, [x12]
  ldrh w15, [x13]
  str w15, [x12]
  ldrh w15, [x13]
  sub x0, x0, 5
  cmp x0, 0
  b.gt stlftest32_loop
  ldp x12, x13, [sp, #0x10]
  ldp x14, x15, [sp, #0x10]
  add sp, sp, #0x40
  ret

matchedstlftest:
  sub sp, sp, #0x40
  stp x14, x15, [sp, #0x10]
  stp x12, x13, [sp, #0x20]  /* x12 = store ptr, x13 = load ptr */
  ldr x15, [x1]
  ldr w12, [x1]
  ldr w13, [x1, 4]
  add x12, x12, x1
  add x13, x13, x1
matchedstlftest_loop:
  str x15, [x12]
  ldr x15, [x13]
  str x15, [x12]
  ldr x15, [x13]
  str x15, [x12]
  ldr x15, [x13]
  str x15, [x12]
  ldr x15, [x13]
  str x15, [x12]
  ldr x15, [x13]
  sub x0, x0, 5
  cmp x0, 0
  b.gt matchedstlftest_loop
  ldp x12, x13, [sp, #0x10]
  ldp x14, x15, [sp, #0x10]
  add sp, sp, #0x40
  ret
