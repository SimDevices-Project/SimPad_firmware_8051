echo "clean outputs..."
if ((ls ./out/) -ne $null) {
    rm -r ./out/
}
mkdir ./out/

$hex = "./out/main.hex"
$bin = "./out/main.bin"

$sources = ls *.c;
if ($sources -ne $null) {
    echo "compiling sources..."
    foreach ($s in $sources) {
        echo "CC: $s"
        sdcc -mmcs51 -c $s -o ./out/
    }
    
    $objs = ls ./out/*.rel
    if ($objs -ne $null) {
        echo "linking objects..."
        sdcc $objs -o $hex
        echo "making binary..."
        makebin $hex $bin

        echo "hex file at: $hex"
        echo "bin file at: $bin"
    }
}
