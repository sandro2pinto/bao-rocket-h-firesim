ENTRY(_start)

SECTIONS
{   
    .nloader : {
        KEEP(*(.nloader))
    }
    
    . =  ALIGN(0x1000);
    .config : ALIGN(8) { 
        __config_start = .;
        KEEP(*(.config)) 
        __config_end = .;
    } 

    . =  ALIGN(0x1000);
    .bao : { 
        __bao_start = .;
        KEEP(*(.bao)) 
        __bao_end = .;
    }  

}
