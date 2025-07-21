        ldr     w0, set_output7
        ldr     w1, output_addr
        str     w0, [w1]
main_loop:
        ldr     w0, set_pin7
        ldr     w1, set_addr
        str     w0, [w1]
        ldr     w2, wait_time 
wait_loop1:      
        subs    w2, w2, #0x1 
        b.ne    wait_loop1
        ldr     w0, set_pin7
        ldr     w1, clear_addr
        str     w0, [w1]
        ldr     w2, wait_time 
wait_loop2:      
        subs    w2, w2, #0x1 
        b.ne    wait_loop2
        b       main_loop
set_output7:
        .int    0x200000
set_pin7:
        .int    0x80
wait_time:
        .int    0x500000
output_addr:
        .int    0x3f200000       
set_addr:
        .int    0x3f20001c       
clear_addr:
        .int    0x3f200028
