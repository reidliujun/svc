-l SMT_C64.lib
-l csl6416.lib



SECTIONS
{

.my_sect 	> 	ISRAM
.my_sect_ext 	> 	SDRAM_CACHABLE
.my_sect_cachable_ext 	> 	SDRAM_CACHABLE
   

}
