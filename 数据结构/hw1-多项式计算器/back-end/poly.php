<?php
header("Access-Control-Allow-Origin: http://home.ustc.edu.cn");
$op = $_GET['op'];
$p1 = $_GET['p1'];
$p2 = $_GET['p2'];
$p3 = $_GET['p3'];
$value = $_GET['value'];
$count1 = $_GET['count1'];
$count2 = $_GET['count2'];
$count3 = $_GET['count3'];
if ($op == '0') {
    $p = './testmain 0 ' . $count1 . ' ' . $count2 . ' ' . $p1 . ' ' . $p2;
} elseif ($op == '1') {
    $p = './testmain 1 ' . $count1 . ' ' . $count2 . ' ' . $p1 . ' ' . $p2;
} elseif ($op == '2') {
    $p = './testmain 2 ' . $count1 . ' ' . $count2 . ' ' . $p1 . ' ' . $p2;
} elseif ($op == '3') {
    $p = './testmain 3 ' . $count3 . ' ' . $value . ' ' . $p3;
} elseif ($op == '4') {
    $p = './testmain 4 ' . $count3 . ' 0 ' . $p3;
} elseif ($op == '5') {
    $p = './testmain 5 ' . $count1 . ' ' . $count2 . ' ' . $p1 . ' ' . $p2;
}
$last_line = system($p, $return_val);