$path="$env:Homepath/Desktop"
$count=3

cd $path
ls | Sort-Object -Property Length | Select-Object -First $count

wait-Event -timeout 10