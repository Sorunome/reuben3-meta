<?php
/*
if(!$security->isLoggedIn() || !($user_info['power']&32)){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
*/
$sql->switchDb('soru_reuben3_meta');
include_once(realpath(dirname(__FILE__)).'/functions.php');
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(E_ALL);
if(isset($_GET['scale'])){
	$scale = (int)$_GET['scale'];
}else{
	$scale = 4;
}
if($scale>10){
	$scale = 10;
}
function createColors(&$im){
	global $white,$lightgray,$darkgray,$black;
	
	$white = imagecolorallocate($im, 0xFF, 0xFF, 0xFF);
	$lightgray = imagecolorallocate($im, 0xC0, 0xC0, 0xC0);
	$darkgray = imagecolorallocate($im, 0x60, 0x60, 0x60);
	$black = imagecolorallocate($im, 0x00, 0x00, 0x00);
}

$SPRITESROOT = '/var/www/www.sorunome.de/reuben3-meta/sprites/';

$tileBuffer = [];
$imgBuffer = [];
function dispTile(&$im,$x,$y,$id){
	global $scale, $sql, $white, $lightgray, $darkgray, $black, $tileBuffer, $imgBuffer, $SPRITESROOT;
	if (!is_array($id)) {
		if (isset($tileBuffer[(int)$id])) {
			$r = $tileBuffer[(int)$id];
		} else {
			$r = $sql->query("SELECT `buffer1`,`buffer2`,`id` FROM `sprites` WHERE `id`=%d",[(int)$id],0);
			$tileBuffer[(int)$id] = $r;
		}
	} else {
		$r = $id;
	}
	$id = (int)$r['id'];
	if (file_exists($SPRITESROOT.(string)$id.'.png') && !isset($imgBuffer[$id])) {
		$imgBuffer[$id] = @imagecreatefromstring(file_get_contents($SPRITESROOT.(string)$id.'.png'));
	}
	if (isset($imgBuffer[$id]) && $imgBuffer[$id]) {
		$i = imagescale($imgBuffer[$id], 8*$scale, 8*$scale);
		imagecopy($im, $i, $x, $y, 0, 0, 8*$scale, 8*$scale);
		imagedestroy($i);
		return;
	}
	//var_dump($r);
	$buffer1bin = hexstr2binstr($r['buffer1']);
	$buffer2bin = hexstr2binstr($r['buffer2']);
	$k = 0;
	for($j = 0;$j < 8;$j++){
		for($i = 0;$i < 8;$i++){
			$s = $buffer1bin[$k].$buffer2bin[$k];
			switch($s){
				case '00':
					imagefilledrectangle($im,$i*$scale+$x,$j*$scale+$y,$i*$scale+$scale+$x,$j*$scale+$scale+$y,$white);
					break;
				case '01':
					imagefilledrectangle($im,$i*$scale+$x,$j*$scale+$y,$i*$scale+$scale+$x,$j*$scale+$scale+$y,$lightgray);
					break;
				case '10':
					imagefilledrectangle($im,$i*$scale+$x,$j*$scale+$y,$i*$scale+$scale+$x,$j*$scale+$scale+$y,$darkgray);
					break;
				case '11':
					imagefilledrectangle($im,$i*$scale+$x,$j*$scale+$y,$i*$scale+$scale+$x,$j*$scale+$scale+$y,$black);
					break;
			}
			
			$k++;
		}
	}
}
function dispTileMap(&$im,$id,$xg,$yg){
	global $scale,$sql;
	$r = $sql->query("SELECT `data` FROM `tilemaps` WHERE `id`=%d",[(int)$id],0);
	$x = 0;
	$y = 0;
	$tiles = json_decode($r['data'],true);
	foreach($tiles as $t){
		//echo ($x)." ".($y)." ".$t."\n";
		dispTile($im,$x*8*$scale+$xg,$y*8*$scale+$yg,$t);
		$x++;
		if($x==12){
			$x = 0;
			$y++;
		}
	}
}
function dispWorld(&$im,$id,$x,$y){
	global $scale,$sql;
	$bounds = $sql->query("SELECT MAX(`x`) as maxx,MAX(`y`) as maxy,MIN(`x`) as minx,MIN(`y`) as miny FROM `tilemaps` WHERE `mapId`=%d",[(int)$id],0);
	$maps = $sql->query("SELECT `x`,`y`,`id` FROM `tilemaps` WHERE `mapId`=%d",[(int)$id]);
	foreach($maps as $m){
		dispTileMap($im,$m['id'],$x+8*12*$scale*($m['x'] - $bounds['minx']),$y+8*8*$scale*($m['y'] - $bounds['miny']));
	}
}


if(isset($_GET['sprite']) && (int)$_GET['sprite'] == $_GET['sprite']){
	$im = @imagecreate(8*$scale,8*$scale) or die('Cannot Initialize GD');
	createColors($im);
	header('Content-Type: image/png');
	dispTile($im,0,0,(int)$_GET['sprite']);
	imagepng($im);
	imagedestroy($im);
}elseif(isset($_GET['map']) && (int)$_GET['map'] == $_GET['map']){
	$im = @imagecreate(12*8*$scale,8*8*$scale) or die('Cannot Initialize GD');
	createColors($im);
	header('Content-Type: image/png');
	dispTileMap($im,(int)$_GET['map'],0,0);
	imagepng($im);
	imagedestroy($im);
}elseif(isset($_GET['world']) && (int)$_GET['world'] == $_GET['world']){
	$bounds = $sql->query("SELECT MAX(`x`) as maxx,MAX(`y`) as maxy,MIN(`x`) as minx,MIN(`y`) as miny FROM `tilemaps` WHERE `mapId`=%d",[(int)$_GET['world']],0);
	$im = @imagecreate(8*12*$scale*($bounds['maxx'] - $bounds['minx']+1),8*8*$scale*($bounds['maxy'] - $bounds['miny']+1)) or die('Cannot Initialize GD');
	$transparentgreen = imagecolorallocate($im,36,135,36);
	imagefill($im,0,0,$transparentgreen);
	createColors($im);
	header('Content-Type: image/png');
	dispWorld($im,(int)$_GET['world'],0,0);
	imagecolortransparent($im,$transparentgreen);
	imagepng($im);
	imagedestroy($im);
}elseif(isset($_GET['spritesheet'])){
	$tiles = $sql->query("SELECT `buffer1`,`buffer2`,`id` FROM `sprites` ORDER BY `id` ASC");
	$maxId = (int)$tiles[sizeof($tiles)-1]['id'];
	$im = @imagecreate(8*15*$scale,8*(floor($maxId / 15)+1)*$scale) or die ('Cannot Initialize GD');
	createColors($im);
	header('Content-Type: image/png');
	foreach($tiles as $t){
		dispTile($im,8*($t['id'] % 15)*$scale,8*floor($t['id'] / 15)*$scale,$t);
	}
	imagepng($im);
	imagedestroy($im);
}
