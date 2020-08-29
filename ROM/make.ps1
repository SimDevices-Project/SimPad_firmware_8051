echo "clean outputs..."
if (Test-Path "./out/") {
    rm -r ./out/
}
$null = mkdir ./out/

$main = "main.c"
$hex = "./out/main.hex"
$bin = "./out/main.bin"

$iram = 256
$xram = 1024
$code = 0x3000
$xram_start = 0x0108 # 这里要看usb.c内的DMA缓冲区结束地址

$sources = ls *.c
if ($sources -ne $null) {
    echo "compiling sources..."
    foreach ($s in $sources) {
        if ($s -match $main) {
            continue
        }
        echo "CC: $s"
        sdcc --std-c11 -mmcs51 --model-small --iram-size $iram --xram-size $xram --code-size $code -c $s -o ./out/
    }
    
    $objs = ls ./out/*.rel
    if ($objs -ne $null) {
        echo "linking objects..."
        sdcc --std-c11 -mmcs51 --model-small --iram-size $iram --xram-size $xram --code-size $code --xram-loc $xram_start $main $objs -o $hex
        echo "making binary..."
        makebin -s $code $hex $bin

        echo "hex file at: $hex"
        echo "bin file at: $bin"
    }
}
