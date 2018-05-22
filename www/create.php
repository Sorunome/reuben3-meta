<?php

if((!$security->isLoggedIn() || !($user_info['power']&32)) && $_GET['pwd']!='vohphohDu0iekie7uc5leis2yohpha7KeebeaShiepei8airee'){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$lang_BACKUP = $lang;
$sql->switchDb('soru_reuben3_meta');
include_once(realpath(dirname(__FILE__)).'/functions.php');
include_once(realpath(dirname(__FILE__)).'/script.php');

chdir(realpath(dirname(__FILE__)));

$mapIds = [];

$mapBlocksPerPage = 26;
$textBlocksPerPage = 23;
$bigSpritesPerPage = 65;
$languages = ['en', 'de', 'fr'];


function dechexpad2($i){
	return str_pad(dechex($i),2,'0',STR_PAD_LEFT);
}
if (!function_exists('dechexpad')) {
function dechexpad($i, $size){
	return str_pad(dechex($i),$size,'0',STR_PAD_LEFT);
}
}
function hex2binstr($s) {
	return str_pad(base_convert($s, 16, 2), 8, '0', STR_PAD_LEFT);
}
function addSpriteSheet($name){
	global $spritesWithId,$spritesInUse,$idCounter,$sql,$spritesLUT,$defines;
	$sheet = json_decode($sql->query("SELECT `sprites` FROM `spritesheets` WHERE `name`='%s'",[$name],0)['sprites'],true)['sprites'];
	
	$name = strtolower($name);
	$defines['sprite_first_'.$name] = '0x'.dechexpad($idCounter, 4);
	foreach($sheet as $s){
		if(in_array($s,$spritesInUse) && !isset($spritesLUT[$s])){
			$asmId = dechexpad($idCounter, 4);
			//$sql->query("UPDATE `sprites` SET `asm_id`='%s' WHERE `id`=%d",[$asmId,$s]);
			
			$spritesLUT[$s] = $asmId;
			$idCounter++;
			$spritesWithId[] = $s;
		}
	}
	$defines['sprite_after_'.$name] = '0x'.dechexpad($idCounter, 4);
}

function compress($hexData, $chunksize = 2){
	foreach($hexData as &$h){
		if(strpos($h,'0x') !== false){
			$h = str_replace('0x','',$h);
		}else{
			$h = dechexpad((int)$h, $chunksize);
		}
	}
	$s = hex2bin(implode('',$hexData));
	
	file_put_contents(realpath(dirname(__FILE__)).'/tmp',$s);
	
	//shell_exec(realpath(dirname(__FILE__)).'/crmprss.py tmp tmpComp');
	shell_exec('./apack c tmp tmpComp');
	
	
	$hexDataComp = str_split(bin2hex(file_get_contents(realpath(dirname(__FILE__)).'/tmpComp')),2);
	foreach($hexDataComp as &$val){
		$val = '0x'.$val;
	}
	return $hexDataComp;
}

function getTextASM($s) {
	$s = mb_convert_encoding($s, 'Windows-1252');
	$s = str_replace("\r\n", "\n", $s);
	
	$s = str_replace("\n\n\n", "\x80", $s); // block wrap
	$s = str_replace("\n\n", "\x81", $s); // page wrap
//	$s = str_replace("\n","\n",$s);
	$s .= "\x00"; // end
	if (strpos($s, "{#ask}\n") !== false) {
		$s = str_replace("{#ask}\n", "\x82", $s); // question
		$i = strpos($s, "\x82") + 1;
		$ss = explode(' ', substr($s, $i)); // $ss will hold all the shop options
		$s = str_replace('_', ' ', $s);
		$i = 0;
		$s .= "\x83".sizeof($ss).','; // shopoptions
		foreach ($ss as $si) {
			$s .= (string)($i*4).',';
			$i += strlen($si) + 1; // +1 because of the space
		}
	}
	return $s;
}

function rgbToIndex($rgb) {
	$r = ($rgb >> 16) & 0xFF;
	$g = ($rgb >> 8) & 0xFF;
	$b = $rgb & 0xFF;
	$index = [
		[  0,   0,   0], // black
		[  0,  67, 133], // darkblue
		[142,  68, 207], // purple
		[  0, 139,  80], // green
		[207, 142,  68], // brown
		[117,  90,  57], // darkgray
		[170, 153, 133], // gray
		[255, 255, 255], // white
		[219,  29,  35], // red
		[255, 168,  17], // orange
		[245, 231,   0], // yellow
		[133, 207,  68], // lightgreen
		[125, 187, 255], // lightblue
		[ 68, 133, 207], // blue
		[207,  68, 133], // pink
		[255, 214, 144], // beige
	];
	for ($i = 0; $i < sizeof($index); $i++) {
		if ($r == $index[$i][0] && $g == $index[$i][1] && $b == $index[$i][2]) {
			return dechex($i);
		}
	}
	$min_index = 0;
	$min_diff = 0xFFFFFF;
	for ($i = 0; $i < sizeof($index); $i++) {
		$diff = abs($index[$i][0] - $r) + abs($index[$i][1] - $g) + abs($index[$i][2] - $b);
		if ($diff < $min_diff) {
			$min_diff = $diff;
			$min_index = $i;
		}
	}
	
	return dechex($min_index);
}
//$sql->query("UPDATE `sprites` SET `in_use`=0 WHERE 1");

foreach($sql->query("SELECT `id` FROM `maps` WHERE `use`=1") as $m){
	$mapIds[] = (int)$m['id'];
}
$spritesInUse = [195,219,261]; // we need open chests and bomb sprites
foreach($sql->query("SELECT `data` FROM `tilemaps` WHERE `mapId` IN (".implode(',',$mapIds).")") as $t){
	$td = json_decode($t['data'],true);
	foreach($td as $s){
		if(!in_array($s,$spritesInUse)){
			$spritesInUse[] = $s;
		}
	}
}

//foreach($spritesInUse as $s){
//	$sql->query("UPDATE `sprites` SET `in_use`=1,`asm_id`=0 WHERE `id`=%d",[$s]);
//}



$spritesWithId = [];
$spritesLUT = [];
$idCounter = 0;
$defines = [];

addSpriteSheet('walk_noswitch');
addSpriteSheet('Walk');
addSpriteSheet('Swim');
addSpriteSheet('Mix_Water');
addSpriteSheet('Water');
addSpriteSheet('hookshot_fly');
addSpriteSheet('hookshot_wall_right');
addSpriteSheet('hookshot_wall_right_up');
addSpriteSheet('hookshot_wall_up');
addSpriteSheet('hookshot_wall_left_up');
addSpriteSheet('hookshot_wall_left');
addSpriteSheet('hookshot_wall_down');
addSpriteSheet('hookshot');
addSpriteSheet('bomb');

foreach($spritesInUse as $s){
	if(!in_array($s,$spritesWithId)){
		$asmId = dechexpad($idCounter, 4);
		//$sql->query("UPDATE `sprites` SET `asm_id`='%s' WHERE `id`=%d",[$asmId,$s]);
		$spritesLUT[$s] = $asmId;
		$idCounter++;
		$spritesWithId[] = $s;
	}
}

$mapDimensions = [
	'width' => 0,
	'minx' => 0,
	'miny' => 0
];
foreach($mapIds as $m){
	$mq = $sql->query("SELECT MIN(`x`) AS `minx`,MAX(`x`)-MIN(`x`)+1 AS `width`,MIN(`y`) AS `miny` FROM `tilemaps` WHERE `mapId`=%d",[(int)$m],0);
	if((int)$mq['width'] > $mapDimensions['width']){
		$mapDimensions['width'] = (int)$mq['width'];
		$mapDimensions['minx'] = (int)$mq['minx'];
		$mapDimensions['miny'] = (int)$mq['miny'];
	}
	$mapDimensions[$m] = [
		'minx' => (int)$mq['minx'],
		'miny' => (int)$mq['miny']
	];
}
foreach($mapIds as $m){
	$mapDimensions[$m]['ox'] = $mapDimensions[$m]['minx'] - $mapDimensions['minx'];
	$mapDimensions[$m]['oy'] = $mapDimensions[$m]['miny'] - $mapDimensions['miny'];
}

$html = '<h1>Sprites</h1><textarea style="width:100%;height:500px;">';

$SPRITESROOT = '/var/www/www.sorunome.de/reuben3-meta/sprites/';
$file = "const uint8_t sprites_data[] = {\n\t8, 8,\n";
$amount = dechexpad(sizeof($spritesWithId), 4);
$file .= "\t0x$amount[2]$amount[3], 0x$amount[0]$amount[1],\n";
$file .= "\t0, 0xFF, 1,\n\n";
$spriteData = [];
foreach($sql->query("SELECT `buffer1`,`buffer2`,`name`,`id` FROM `sprites` WHERE `id` IN (".implode(',',array_map('intval',$spritesWithId)).")") as $s){
	if($s['name']){
		$defines['sprite_'.strtolower($s['name'])] = '0x'.$spritesLUT[$s['id']];
	}
	$out = "";
	if (file_exists($SPRITESROOT.$s['id'].'.png')) {
		$ii = @imagecreatefromstring(file_get_contents($SPRITESROOT.$s['id'].'.png'));
		$i = imagecreatetruecolor(8, 8);
		imagecopy($i, $ii, 0, 0, 0, 0, 8, 8);
		imagedestroy($ii);
		$alt = false;
		for ($y = 0; $y < 8; $y++) {
			for ($x = 0; $x < 8; $x++) {
				$rgb = imagecolorat($i, $x, $y);
				if (!$alt) {
					$out .= '0x';
				}
				$out .= rgbToIndex($rgb);
				if ($alt) {
					$out .= ', ';
				}
				$alt = !$alt;
			}
		}
		imagedestroy($i);
	} else {
		$alt = false;
		for($i = 0;$i < 16;$i += 2){
			$b1 = hex2binstr(substr($s['buffer1'],$i,2));
			$b2 = hex2binstr(substr($s['buffer2'],$i,2));
			for ($j = 0; $j < 8; $j++) {
				$n = ($b1[$j] == '1')*2 + ($b2[$j] == '1')*1;
				if (!$alt) {
					$out .= '0x';
				}
				$out .= ['7', '6', '5', '0'][$n];
				if ($alt) {
					$out .= ', ';
				}
				$alt = !$alt;
			}
		}
	}
	$spriteData[$s['id']] = $out;
}
// we do this to get the order of the sprites correctly
foreach($spritesWithId as $i){
	$file .= "\t".$spriteData[$i]."\n";
}
$file .= "};\n";
$i = 0;
foreach($spritesLUT as $normalId => $asmId){
	$defines['sprite_'.$normalId] = "0x".$asmId;
	$i++;
}
$defines['sprites_total'] = $i;
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/sprites.h',$file);


$html .= $file.'</textarea><h1>Big Sprite Data</h1><textarea style="width:100%;height:500px;">';


$idCounter = 0;
$file = '';
$largeSpritesHeader = "const uint8_t* const EnemySprites[] = {\n";
$defines['enemy_sprites_max_size'] = 0;

$ENEMIESROOT = '/var/www/www.sorunome.de/reuben3-meta/enemies/';

function getBigSpriteData($data,&$file,$idCounter) {
	global $defines, $ENEMIESROOT;
	$file .= "const uint8_t EnemySprites_${idCounter}_buffer[] = {\n\t";
	$size = ($data['width']*8*$data['height'] / 2) + 2;
	if ($size > $defines['enemy_sprites_max_size']) {
		$defines['enemy_sprites_max_size'] = $size;
	}
	$hex = [];
	$s = '';
	$fname = $ENEMIESROOT.str_pad($idCounter, 4, '0', STR_PAD_LEFT).'_color.png';
	if (file_exists($fname)) {
		$d = getimagesize($fname);
		$w = $d[0];
		$h = $d[1];
		$ii = imagecreatefrompng($fname);
		$i = imagecreatetruecolor($w, $h);
		$alphabg = imagecolorallocatealpha($i, 0, 0, 0, 127);
		imagefill($i, 0, 0, $alphabg);
		imagealphablending($i, false);
		imagesavealpha($i, true);
		imagecopy($i, $ii, 0, 0, 0, 0, $w, $h);
		imagedestroy($ii);


		// first determine transparent color
		$transColors = [true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true];
		for($y = 0; $y < $h; $y++) {
			for ($x = 0; $x < $w; $x++) {
				$rgba = imagecolorat($i, $x, $y);
				$alpha = ($rgba >> 24) & 0x7F;
				if (!$alpha) {
					$transColors[hexdec(rgbToIndex($rgba))] = false;
				}
			}
		}
		$transparentColor = -1;
		for($j = 0; $j < 16; $j++) {
			if ($transColors[$j]) {
				$transparentColor = $j;
				break;
			}
		}
		if ($transparentColor == -1) {
			die('nuuuu');
		}
		
		// now actually create the image
		$hex = array_merge($hex, [$w, $h, '0x01', '0x00', '0x00', '0x0'.dechex($transparentColor), '0x01']);
		for($y = 0; $y < $h; $y++) {
			$alt = false;
			$s = '';
			for ($x = 0; $x < $w; $x++) {
				if (!$alt) {
					$s .= '0x';
				}
				$rgba = imagecolorat($i, $x, $y);
				$alpha = ($rgba >> 24) & 0x7F;
				if ($alpha) {
					$s .= dechex($transparentColor);
				} else {
					$s .= rgbToIndex($rgba);
				}
				
				if ($alt) {
					$hex[] = $s;
					$s = '';
				}
				$alt = !$alt;
			}
			if ($alt) {
				$hex[] = $s.'0';
			}
		}
		
		imagedestroy($i);
	} else {
		$hex = array_merge($hex, [$data['width']*8, $data['height'], '01', '00', '00', 'FF', '01']);
		for($i = 0;$i < strlen($data['frontBuf']);$i += 2){
			$b1 = hex2binstr(substr($data['frontBuf'],$i,2));
			$b2 = hex2binstr(substr($data['backBuf'],$i,2));
			$alt = false;
			for ($j = 0; $j < 8; $j++) {
				$n = ($b1[$j] == '1')*2 + ($b2[$j] == '1')*1;
				if (!$alt) {
					$s .= '0x';
				}
				$s .= ['7', '6', '5', '0'][$n];
				if ($alt) {
					$hex[] = $s;
					$s = '';
				}
				$alt = !$alt;
			}
			if ($alt) {
				$hex[] = $s.'0';
				$s = '';
			}
		}
	}
	$hex = compress($hex);
	for($i = 0; $i < sizeof($hex); $i++) {
		$file .= $hex[$i].', ';
	}
	
	$file .= "\n};\n";
}
$enemiesFile = "const Enemy_Data enemies[] = {\n";
$enemiesLUT = [];
$enemies = $sql->query("SELECT `name`,`enemyData`,`id` FROM `enemies` WHERE 1");
foreach($enemies as $e){
	if($e['name']!=''){
		$defines['bigsprite_'.strtolower($e['name'])] = $idCounter;
	}
	$defines['bigsprite_'.$e['id']] = $idCounter;
	
	$largeSpritesHeader .= "\tEnemySprites_${idCounter}_buffer,\n";
	
	$data = json_decode($e['enemyData'],true);
	getBigSpriteData($data,$file,$idCounter);
	$enemiesLUT[(int)$e['id']] = $idCounter;
	$defines['enemy_'.strtolower($e['name'])] = $idCounter;
	$enemiesFile .= "\t{ ".$idCounter.", ".$data['level'].", ";
	$enemiesFile .= $data['hp'].", ".$data['exp'].", ";
	$enemiesFile .= "0x".dechexpad2($data['sl1']*16 + $data['firedef']).", 0x".dechexpad2($data['sl2']*16 + $data['icedef']).", 0x".dechexpad2($data['sl3']*16 + $data['boltdef']).", 0x".dechexpad2($data['sl4']*16 + $data['bombdef']).", 0x".dechexpad2($data['sl5']*16 + ($data['boss']?0:1)).", ";
	$enemiesFile .= "".$data['xpos'].", ".$data['ypos'].", ".$data['wait']." },\n";
	$idCounter++;
}
$enemiesFile .= "};\n";
/*
$largeSprites = $sql->query("SELECT `name`,`data` FROM `bigSprites` WHERE 1");
foreach($largeSprites as $e){
	if($e['name']!=''){
		$defines['bigsprite_'.$e['name']] = $idCounter;
	}
	$defines['bigsprite_'.$idCounter] = $idCounter;
	
	$largeSpritesHeader .= "\tEnemySprites_${idCounter}_buffer,\n";
	
	$data = json_decode($e['data'],true);
	getBigSpriteData($data,$file,$idCounter);
	$idCounter++;
}
*/
$largeSpritesHeader .= "};\n";

$file = $file.$largeSpritesHeader;

file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/largeSprites.h',$file);

$html .= $file.'</textarea><h1>Enemies</h1><textarea style="width:100%;height:500px;">';

file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/enemies.h',$enemiesFile);

$html .= $enemiesFile.'</textarea><h1>Areas</h1><textarea style="width:100%;height:500px;">';
$file = "const uint8_t area_enemies[][10] = {\n";
$areasLUT = [];
$idCounter = 0;
foreach($sql->query("SELECT `id`,`name`,`enemies` FROM `areas` WHERE 1") as $a){
	if($a['id'] !== NULL){
		$areasLUT[(int)$a['id']] = $idCounter;
		$defines['area_'.$a['id']] = $idCounter;
		if($a['name'] != ''){
			$defines['area_'.strtolower($a['name'])] = $idCounter;
			$file .= "\t{";
			if($a['enemies']){
				foreach(json_decode($a['enemies'],true) as $ae){
					if(isset($enemiesLUT[$ae])){
						$file .= $enemiesLUT[$ae].',';
					}else{
						$file .= '255,';
					}
				}
			}else{
				$file .= '255,255,255,255,255,255,255,255,255,255,';
			}
			$file .= "},\n";
		}
		$idCounter++;
	}
}
$file .= "};\n";
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/area_enemies.h',$file);
$html .= $file.'</textarea>';


$html .= '<h1>String Data</h1><textarea style="width:100%;height:500px;">';


$file = '';
$hexData = [];
$uncompressedSize = 0;
$compressedSize = 0;
$bytesCompressed = 0;
$blocknum = [];
$blockLUT = [];
$stringLUT = [];
foreach($languages as $i => $l) {
	$hexData[$i] = [];
	$blocknum[$i] = 0;
	$blockLUT[$i] = "const uint8_t* const _Text_decompression_LUT_${l}[] = {\n";
	$stringLUT[$i] = "const Strings_LUT stringsLut_${l}[] = {\n";
}

$compress_string = function($min, $l, $lang) use (&$file,&$hexData,&$uncompressedSize,&$compressedSize,&$bytesCompressed,&$blocknum,&$blockLUT,&$file2,&$textBlocksPerPage){
	$workData = [];
	$j = 2047;
	for($i = 0;$i < $j; $i++){
		if(!isset($hexData[$l][$i])){
			break;
		}
		$workData[] = $hexData[$l][$i];
		if($hexData[$l][$i] == 'FREEZEBLOCK' || $hexData[$l][$i] == 'UNFREEZEBLOCK'){
			$j++;
		}
		unset($hexData[$l][$i]);
	}
	//$chunks = array_chunk($hexData[$l],827 + $min);
	//echo "precompress length:";
	//var_dump(sizeof($workData));
	
	$chunk_prepend = [];
	if(($i = array_search('FREEZEBLOCK',array_reverse($workData,true))) !== false){
		
		$j = sizeof($workData);
		if(sizeof($hexData[$l]) > 0 && array_search('UNFREEZEBLOCK',array_slice($workData,$i)) === false){ // uho, we'll better flip this over
			for($i++;$i < $j;$i++){
				$chunk_prepend[] = $workData[$i];
				unset($workData[$i]);
			}
		}
	}
	while(($i = array_search('FREEZEBLOCK',$workData)) !== false){
		unset($workData[$i]);
	}
	while(($i = array_search('UNFREEZEBLOCK',$workData)) !== false){
		unset($workData[$i]);
	}
	
	
	$hexData[$l] = array_merge($chunk_prepend,$hexData[$l]);
	
	
	$uncompressedSize += sizeof($workData);
	//echo "compress length:";
	//var_dump(sizeof($workData));
	$workData[] = '0x84'; // end of chunk
	
	$hexDataOut = compress($workData);
	$compressedSize += sizeof($hexDataOut);
	$bytesCompressed += sizeof($workData) - sizeof($hexDataOut) + 5; // just appriximate the # of strings
	$f = "const uint8_t _Text_block_$blocknum[$l]_${lang}[] = {\n\t";
	for($i = 0;$i < sizeof($hexDataOut);$i++){
		$f .= $hexDataOut[$i].',';
	}
	$f .= "\n};\n";
	$blockLUT[$l] .= "\t_Text_block_$blocknum[$l]_$lang,\n";
	$blocknum[$l]++;
	$file .= $f;
};
$defines['first_offpage_textblock'] = $textBlocksPerPage;
$strings = $sql->query("SELECT * FROM `strings` WHERE `compress`=1");
$stringCounter = 0;
foreach($strings as $str) {
	$defines['string_'.strtolower($str['name'])] = $stringCounter;
	$stringCounter++;
	foreach($languages as $l => $lang) {
		$arrayCount = array_count_values($hexData[$l]);
		$min = $arrayCount['FREEZEBLOCK']??0;
		$min += $arrayCount['UNFREEZEBLOCK']??0;
		
		//echo "\n======\n",$str['name']."\n";
		//var_dump(sizeof($hexData));
		//var_dump($min);
		//echo "-----\n";
		$stringLUT[$l] .= "\t{ ".(sizeof($hexData[$l]) - $min).", $blocknum[$l] },\n";
		$bs = '';
		$s = getTextASM($str['string_'.$lang]);
		
		for($i = 0; $i < strlen($s); $i++) {
			$c = substr($s, $i, 1);
			//if($i == 0){
			//	echo $c."\n";
			//}
			if ($c == "\x83") { // we don't split options accross multiple chunks
				$bs .= 'FREEZEBLOCK,'.substr($s, $i+1).'UNFREEZEBLOCK,';
				break;
			}
			if ($c == "\\" && substr($s, $i+1, 1) == 'x') {
				$i += 2;
				$bs .= '0x'.substr($s, $i, 2).',';
				$i++; // the second increase will happen in the for-loop
			} else {
				$bs .= '0x'.dechexpad2(ord($c)).',';
			}
		}
		//echo "new length:";
		//var_dump(sizeof(explode(',',rtrim($bs,','))));
		$hexData[$l] = array_merge($hexData[$l], explode(',', rtrim($bs, ',')));
		
		$arrayCount = array_count_values($hexData[$l]);
		
		$min = $arrayCount['FREEZEBLOCK']??0;
		$min += $arrayCount['UNFREEZEBLOCK']??0;
		while ((sizeof($hexData[$l]) - $min) >= 2047) { // time to compress this, but first look that everything after our flag is in the same block
			$compress_string($min, $l, $lang);
		}
	}
}
foreach($languages as $l => $lang) {
	while (sizeof($hexData[$l])) {
		$compress_string(0, $l, $lang);
	}
}

$defines['total_strings'] = $stringCounter;
foreach($languages as $l => $lang) {
	$blockLUT[$l] .= "};\n";
	$stringLUT[$l] .= "};\n";
	$file .= "\n$blockLUT[$l]\n$stringLUT[$l]";
}

$file .= "\n\nconst Strings_MasterLUT stringsMasterLut[] = {\n";
foreach($languages as $l => $lang) {
	$file .= "\t{ LANG_".strtoupper($lang).", _Text_decompression_LUT_$lang, stringsLut_$lang },\n";
}
$file .= "};";
$defines['number_languages'] = sizeof($languages);
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/strings.h',$file);

$compressedPercent = (($compressedSize/$uncompressedSize)*100);
$html .= $file.'</textarea>Compressed: '.$compressedSize.'/'.$uncompressedSize.' ('.$compressedPercent.'%)<br>Bytes compressed: '.$bytesCompressed;


$html .='<h1>Tilemaps</h1><textarea style="width:100%;height:500px;">';

$eventCounter = 0;
foreach($sql->query("SELECT `offset`,`bit`,`id`,`name` FROM `events`") as $e){
	$defines['event_'.$e['id']] = $eventCounter;
	if($e['name'] != ''){
		$defines['event_'.strtolower($e['name'])] = $eventCounter;
	}
	$eventCounter++;
}
$defines['total_events'] = $eventCounter;
$stringCounter = 0;

$eventTilesLutLut = '';
$eventTilesLut = [];
$file = "/* tilemap data */\n";
$dynTilesCode = "";
$tilemapsLut = '';
$worldLUT = "const World_Data worlds[] = {\n";
$uncompressedSize = 0;
$compressedSize = 0;
$bytesCompressed = 0;

$worldASMId = 0;
$chunknum = 0;
$hexData = [];

$maps = '';
$thisId = 0;
$tilemapIds = [];

foreach($sql->query("SELECT `name`,`id` FROM `maps` WHERE `id` IN (".implode(',',array_map('intval',$mapIds)).")") as $m){
	$name = $m['name'];
	$mapId = (int)$m['id'];
	
	$defines['world_'.strtolower($name)] = $worldASMId;
	$defines['world_'.$mapId] = $worldASMId;
	
	// we adjust the pointers here as in the parsing we'd have to decrease them else first
	$worldLUT .= "\t{ tilemaps_$worldASMId, ";
	$worldLUT .= "dynTilesStart_$worldASMId, ";
	$worldLUT .= "actionTilesStart_$worldASMId },\n";
	
	$tilemaps = $sql->query("SELECT `data`,`id`,`x`,`y`,`area`,`enemies` FROM `tilemaps` WHERE `mapId`=%d",[$mapId]);
	$tilemapsLut .= "const Tilemaps_Data tilemaps_${worldASMId}[] = {\n";
	
	$dyntilesOnWorld = false;
	$eventTilesOnWorld = false;
	
	foreach($tilemaps as $t){
		$tilemapIds[] = (int)$t['id'];
		$td = json_decode($t['data'],true);
		$k = 0;
		$xPos = (int)$t['x'] - $mapDimensions[(int)$m['id']]['ox'] - $mapDimensions['minx'];
		$yPos = (int)$t['y'] - $mapDimensions[(int)$m['id']]['oy'] - $mapDimensions['miny'];
		$mapId = $yPos * $mapDimensions['width'] + $xPos;
		//$sql->query("UPDATE `tilemaps` SET `asm_id`='%s' WHERE `id`=%d",[dechexpad2($mapId),$t['id']]);
		
		
		$defines['tilemap_'.$t['id']] = $mapId;
		$tileEvents = $sql->query("SELECT `id`,`x`,`y`,`code`,`add_jump` FROM `eventTiles` WHERE `refId`=%d",[$t['id']]);
		$dynTiles = $sql->query("SELECT t1.`id`,t1.`x`,t1.`y`,t1.`newTile`,t2.`id` AS eventId FROM `dynTiles` AS t1 INNER JOIN `events` AS t2 ON t1.`event`=t2.`id` WHERE t1.`refId`=%d",[$t['id']]);
		if(isset($areasLUT[(int)$t['area']])){
			$headerByte = $areasLUT[(int)$t['area']];
		}else{
			$headerByte = 0;
		}
		if($t['enemies']){
			$headerByte |= 0x80;
		}
		
		if($tileEvents[0]['id'] !== NULL){
			if(!$eventTilesOnWorld){
				$eventTilesLutLut .= "const Actiontiles_LUTLUT actionTilesStart_${worldASMId}[] = {\n";
				$eventTilesOnWorld = true;
			}
			$eventTilesLutLut .= "\t{ 0x".dechexpad2($mapId).", ".sizeof($tileEvents).", event_tiles_LUT_for_map_$t[id] },\n";
			$s = '';
			foreach($tileEvents as $te){
				$s .= "\t{ ".(string)(((int)$te['y'] * 12) + (int)$te['x']).", event_tile_routine_$te[id] },\n";
			}
			$eventTilesLut['event_tiles_LUT_for_map_'.$t['id']] = $s;
		}
		if($dynTiles[0]['id'] !== NULL){
			if(!$dyntilesOnWorld){
				$dynTilesCode .= "const Dyn_Data dynTilesStart_${worldASMId}[] = {\n";
				$dyntilesOnWorld = true;
			}
			foreach($dynTiles as $d){
				// offset is *2 because we have uint8_t decompression buffer but our map is uint16_t and we do the modifying on the decompression buffer
				$dt = $spritesLUT[$d['newTile']];
				$dynTilesCode .= "\t{ 0x".dechexpad2($mapId).", ".$defines['event_'.$d['eventId']].", ".(($d['y']*12 + $d['x'])*2).", 0x$dt[2]$dt[3], 0x$dt[0]$dt[1]},\n";
			}
		}
		if ($chunknum == 0) {
			$thisId = $t['id'];
		}
		$tilemapsLut .= "\t{ 0x".dechexpad2($mapId).", ";
		$tilemapsLut .= "0x".dechexpad2($headerByte).", ";
		$tilemapsLut .= "$chunknum, ";
		$tilemapsLut .= "_tilemaps_data_$thisId },\n";
		
		$chunknum++;
		for ($i = 0;$i < 8;$i++) {
			for ($j = 0;$j < 12;$j++) {
				$bytes = $spritesLUT[$td[$k]];
				$hexData[] = '0x'.$bytes[2].$bytes[3].$bytes[0].$bytes[1]; // swap endian-ness
				$k++;
			}
		}
		if ($chunknum == 8) {
			// time to compress this thing!
			$chunknum = 0;
			$hexDataOut = compress($hexData, 4);
			$uncompressedSize += sizeof($hexData);
			$compressedSize += sizeof($hexDataOut);
			
			$bytesCompressed += sizeof($hexData) - sizeof($hexDataOut) - 3; // pointer size
			$maps .= "const uint8_t _tilemaps_data_${thisId}[] = {";
			for($i = 0;$i < sizeof($hexDataOut);$i++){
				$maps .= $hexDataOut[$i].',';
			}
			$maps .= "};\n";
			
			$hexData = [];
		}
	}
	$tilemapsLut .= "};\n";
	
	
	if ($dyntilesOnWorld) {
		$dynTilesCode .= "\t{ 0xFF, 0, 0, 0, 0 },\n};\n";
	}
	if ($eventTilesOnWorld) {
		$eventTilesLutLut .= "\t{ 0xFF, 0, 0 },\n};\n";
	} else {
		$defines['actionTilesStart_'.$worldASMId] = 0;
	}
	$worldASMId++;
}
if ($chunknum != 0) { // we still have more to compress!
	$hexDataOut = compress($hexData, 4);
	$uncompressedSize += sizeof($hexData);
	$compressedSize += sizeof($hexDataOut);
	
	$bytesCompressed += sizeof($hexData) - sizeof($hexDataOut) - 3; // pointer size
	$maps .= "const uint8_t _tilemaps_data_${thisId}[] = {";
	for ($i = 0; $i < sizeof($hexDataOut); $i++) {
		$maps .= $hexDataOut[$i].',';
	}
	$maps .= "};\n";
}
$worldLUT .= "};\n";

// now create the scripts (couldn't do that before because we need the defines)
// define some constants
$defines['tradequest_none'] = 0;
$defines['tradequest_letter1'] = 1;
$defines['tradequest_letter2'] = 2;
$defines['tradequest_food'] = 3;
$defines['tradequest_fishing_rod'] = 4;
$defines['tradequest_fish'] = 5;
$defines['tradequest_necklace'] = 6;
$defines['tradequest_shell'] = 7;
$defines['tradequest_rug'] = 8;
$defines['tradequest_fox'] = 9;
$defines['tradequest_done'] = 10;
$defines['weapon_none'] = 0;
$defines['weapon_stick'] = 2;
$defines['weapon_axe'] = 4;
$defines['weapon_sword'] = 5;
$defines['weapon_breadsword'] = 6;
$defines['weapon_toxicsword'] = 7;
$defines['armor_wounds'] = 1;
$defines['armor_none'] = 2;
$defines['armor_shield'] = 4;
$defines['i_item_none'] = 0;
$defines['i_item_bottle1'] = 1;
$defines['i_item_bottle2'] = 2;
$defines['i_item_bottle3'] = 3;
$defines['i_item_bottle4'] = 4;
$defines['i_item_hookshot'] = 5;
$defines['i_item_bomb'] = 6;
$defines['i_item_protect'] = 7;

$defines['bottle_content_empty'] = 0;
$defines['bottle_content_dirty_water'] = 1;
$defines['bottle_content_water'] = 2;
$defines['bottle_content_herb'] = 3;
$defines['bottle_content_potion'] = 4;
$defines['bottle_content_ginseng'] = 5;
$defines['bottle_content_elixir'] = 6;

$defines['script_walk'] = 0;
$defines['script_action'] = 1;
$defines['script_bomb'] = 2;
$defines['script_pushblock'] = 3;

$defines['sound_chest'] = 0;
$defines['sound_badchest'] = 1;

$scriptfile = '';
$scriptfileLut = "const uint8_t* scripts[] = {\n";
$scriptId = 0;
foreach ($sql->query("SELECT `id`, `name`, `code` FROM `scripts`") as $s) {
	$defines['script_'.strtolower($s['name'])] = $scriptId;
	$defines['script_'.(string)$s['id']] = $scriptId;
	$scriptId++;
}
$scriptId = 0;
foreach ($sql->query("SELECT `id`, `name`, `code` FROM `scripts`") as $s) {
	$s['code'] .= "\nreturn(true)";
	
	$code = bin2hex($parser->parse($s['code'], 0, $defines));
	$scriptfile .= "const uint8_t script_${scriptId}[] = {\n\t";
	if ($code == '') {
		$code = 'FF';
	}
	foreach (str_split($code, 2) as $h) {
		$scriptfile .= "0x$h, ";
	}
	$scriptfile .= "\n};\n";
	$scriptfileLut .= "\tscript_$scriptId,\n";
	
	$scriptId++;
}
$scriptfileLut .= "};\n";
$scriptfile .= $scriptfileLut;
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/scripts.h', $scriptfile);


foreach ($sql->query("SELECT `id`,`x`,`y`,`code`,`add_jump` FROM `eventTiles` WHERE `refId` IN (".implode(',',array_map('intval',$tilemapIds)).")") as $te) {
	if ($te['add_jump']) {
		$te['code'] .= "\nreturn(true)";
	}
	$code = bin2hex($parser->parse($te['code'], 0, $defines));
	$file .= "const uint8_t event_tile_routine_".$te['id']."[] = {\n\t";
	if ($code == '') {
		$code = 'FF';
	}
	foreach (str_split($code, 2) as $h) {
		$file .= "0x$h, ";
	}
	$file .= "\n};\n";
}

foreach($eventTilesLut as $label => $value){
	$file .= "const Actiontiles_LUT ${label}[] = {\n$value};\n";
}
$file .= $eventTilesLutLut."\n\n";
$file .= $maps;
$file .= $tilemapsLut;
$file .= "\n\n\n";

$file .= $dynTilesCode;
$file .= "\n\n\n";
$file .= $worldLUT;
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/maps.h',$file);


$defines['tilemaps_header_size'] = '2';
$defines['tilemaps_width'] = $mapDimensions['width'];
//$defaultMap = $sql->query("SELECT `asm_id` FROM `tilemaps` WHERE `id`=%d",[$vars->get('defaultMap')],0);
//$defines['tilemaps_defaultmap'] = "$".$defaultMap['asm_id'];


$compressedPercent = (($compressedSize/$uncompressedSize)*100);
$html .= $file.'</textarea>Compressed: '.$compressedSize.'/'.$uncompressedSize.' ('.$compressedPercent.'%)<br>Bytes compressed: '.$bytesCompressed;

$html .= '<h1>Defines</h1><textarea style="width:100%;height:500px;">';


$file = "";
foreach($defines as $label => $value){
	$label = strtoupper($label);
	$file .= "#define $label $value\n";
}
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/defines.h',$file);


$html .= $file.'</textarea><hr><a href="/reuben3-meta">&lt;&lt; Back</a>';

$sql->switchDb('soru_homepage');
$lang = $lang_BACKUP;
echo $page->getPage('Create Asm',$html,$lang,$pathPartsParsed);
