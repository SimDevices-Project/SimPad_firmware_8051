function clean_dir($params) {
    if (Test-Path $params) {
        Remove-Item -r $params
    }
    $null = mkdir $params
}

Write-Output "clean outputs..."
clean_dir("./out/")

$main = "main.c"
$out = "./out"
$tmp = "./out/tmp"

$iram = 256
$xram = 1024
$code = 0x3000
$xram_start = 0x0108 # 这里要看usb.c内的DMA缓冲区结束地址

function compile($params) {
    $sources = $params[0]
    $target = $params[1]
    
    $defines = ""
    $name = "simpad"

    if ($null -ne $target) {
        $defines = "-D__MULTI_COMPILE__", "-D$target"
        $name = $target.ToLower()
        Write-Output "compiling sources for $name"
    } else {
        Write-Output "compiling sources..."
    }

    clean_dir($tmp)
    foreach ($s in $sources) {
        if ($s -match $main) {
            continue
        }
        if ($null -eq $target) {
            Write-Output "CC: $s"
        }
        sdcc --std-c11 -mmcs51 --opt-code-size --model-large --iram-size $iram --xram-size $xram --code-size $code --xram-loc $xram_start $defines -c "$s" -o "$tmp/"
    }
    
    $hex = "$out/$name.hex"
    $bin = "$out/$name.bin"
    $objs = Get-ChildItem "$tmp/*.rel"
    if ($null -ne $objs) {
        if ($null -eq $target) {
            Write-Output "linking objects..."
        }
        sdcc --std-c11 -mmcs51 --opt-code-size --model-large --iram-size $iram --xram-size $xram --code-size $code --xram-loc $xram_start "$main" $objs -o "$tmp/name.hex"
        Copy-Item "$tmp/name.hex" $hex
        if ($null -eq $target) {
            Write-Output "making binary..."
        }
        makebin -s $code "$hex" "$bin"

        if ($null -eq $target) {
            Write-Output "hex file at: $hex"
            Write-Output "bin file at: $bin"
        }
    }
}

$targets = "SIMPAD_V2_AE", "SIMPAD_NANO_AE", "SIM_KEY", "SIMPAD_V2", "SIMPAD_NANO"

$sources = Get-ChildItem -Path "./" -File -Name *.c
if ($null -ne $sources) {
    if ($args -ne $null) {
        switch ($args[0].ToLower()) {
            "all" { 
                Write-Output "building for targets: $targets"
                foreach ($s in $targets) {
                    compile($sources, $s)
                }
             }
             # TODO: 添加其他目标？
            Default {
                Write-Output "Incorrect param, or you want to use 'all' ?"
            }
        }
    } else {
        compile($sources, $null)
    }
}

if (Test-Path $tmp) {
    Remove-Item -r $tmp
}
