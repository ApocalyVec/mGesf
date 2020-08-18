
SECTIONS
{
    _sdram_lma = .;
    sdram_region (NOLOAD):
    AT ( _sdram_lma )
    {
        *(sdram_region)
    } >sdram
}
