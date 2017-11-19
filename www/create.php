<?php

if((!$security->isLoggedIn() || !($user_info['power']&32)) && $_GET['pwd']!='vohphohDu0iekie7uc5leis2yohpha7KeebeaShiepei8airee'){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$sql->switchDb('soru_reuben3_meta');
include_once(realpath(dirname(__FILE__)).'/functions.php');
include_once(realpath(dirname(__FILE__)).'/script.php');

chdir(realpath(dirname(__FILE__)));

$mapIds = [];

$mapBlocksPerPage = 26;
$textBlocksPerPage = 23;
$bigSpritesPerPage = 65;


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
		if(in_array($s,$spritesInUse)){
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

addSpriteSheet('Walk');
addSpriteSheet('Swim');
addSpriteSheet('Mix_Water');
addSpriteSheet('Water');
addSpriteSheet('hookshotfly');
addSpriteSheet('hookshotgrab');
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


//$sql->query("UPDATE `font` SET `use`=0,`asm_id`=0 WHERE 1");
$texts = $sql->query("SELECT `string` FROM `strings` WHERE 1");
$charsInUse = [];
$texts[] = ['string' => '0123456789']; // make sure the numbers get loaded
foreach($texts as $s){
	$s = getTextASM($s['string']);
	for($i = 0;$i < strlen($s);$i++){
		$c = substr($s,$i,1);
		if($c == "\x05"){
			break;
		}
		if(!in_array($c,$charsInUse)){
			$charsInUse[] = $c;
		}
	}
}
/*
foreach($charsInUse as $c){
	$sql->query("UPDATE `font` SET `use`=1 WHERE `char`='%s'",[$c]);
}
$sql->query("UPDATE `font` SET `asm_id`=1 WHERE `char`='0'");
$sql->query("UPDATE `font` SET `asm_id`=2 WHERE `char`='1'");
$sql->query("UPDATE `font` SET `asm_id`=3 WHERE `char`='2'");
$sql->query("UPDATE `font` SET `asm_id`=4 WHERE `char`='3'");
$sql->query("UPDATE `font` SET `asm_id`=5 WHERE `char`='4'");
$sql->query("UPDATE `font` SET `asm_id`=6 WHERE `char`='5'");
$sql->query("UPDATE `font` SET `asm_id`=7 WHERE `char`='6'");
$sql->query("UPDATE `font` SET `asm_id`=8 WHERE `char`='7'");
$sql->query("UPDATE `font` SET `asm_id`=9 WHERE `char`='8'");
$sql->query("UPDATE `font` SET `asm_id`=10 WHERE `char`='9'");*/


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
$file = "const uint16_t sprites_data[] = {\n\t8, 8,\n";
$spriteData = [];
foreach($sql->query("SELECT `buffer1`,`buffer2`,`name`,`id` FROM `sprites` WHERE `id` IN (".implode(',',array_map('intval',$spritesWithId)).")") as $s){
	if($s['name']){
		$defines['sprite_'.$s['name']] = '0x'.$spritesLUT[$s['id']];
	}
	$out = "";
	if (file_exists($SPRITESROOT.$s['id'].'.png')) {
		$ii = @imagecreatefromstring(file_get_contents($SPRITESROOT.$s['id'].'.png'));
		$i = imagecreatetruecolor(8, 8);
		imagecopy($i, $ii, 0, 0, 0, 0, 8, 8);
		imagedestroy($ii);
		for ($y = 0; $y < 8; $y++) {
			for ($x = 0; $x < 8; $x++) {
				$rgb = imagecolorat($i, $x, $y);
				$r = ($rgb >> 16) & 0xFF;
				$g = ($rgb >> 8) & 0xFF;
				$b = $rgb & 0xFF;
				$c = (($r & 0xF8) << 8) | (($g & 0xFC) << 3) | ($b >> 3);
				$out .= '0x'.dechexpad($c, 4).',';
			}
		}
		imagedestroy($i);
	} else {
		for($i = 0;$i < 16;$i += 2){
			$b1 = hex2binstr(substr($s['buffer1'],$i,2));
			$b2 = hex2binstr(substr($s['buffer2'],$i,2));
			for ($j = 0; $j < 8; $j++) {
				$n = ($b1[$j] == '1')*2 + ($b2[$j] == '1')*1;
				$out .= ['0xFFFF', '0xACD0', '0x72C7', '0x0000'][$n].',';
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
$largeSpritesHeader = "EnemySprites:\n";

function getBigSpriteData($data,&$file,$idCounter){
	$file .= "EnemySprites_".$idCounter."_Size:\n\t.db ".($data['width']*8).','.$data['height']."\n\t.db ";
	for($i = 0;$i < strlen($data['frontBuf']);$i += 2){
		$file .= "$".substr($data['frontBuf'],$i,2).",";
		$file .= "$".substr($data['backBuf'],$i,2).",";
	}
	
	$file = substr($file,0,-1)."\n";
}
$enemiesFile = "enemies:\n";
$enemiesLUT = [];
$enemies = $sql->query("SELECT `name`,`enemyData`,`id` FROM `enemies` WHERE 1");
foreach($enemies as $e){
	if($e['name']!=''){
		$defines['bigsprite_'.$e['name']] = $idCounter;
	}
	$defines['bigsprite_'.$e['id']] = $idCounter;
	
	$largeSpritesHeader .= "\t.dw EnemySprites_".$idCounter."_Size\n";
	
	$data = json_decode($e['enemyData'],true);
	getBigSpriteData($data,$file,$idCounter);
	$enemiesLUT[(int)$e['id']] = $idCounter;
	$defines['enemy_'.$e['name']] = $idCounter;
	$enemiesFile .= "\n\t.db ".$idCounter.",".$data['level']."\n";
	$enemiesFile .= "\t.dw ".$data['hp'].",".$data['exp']."\n";
	$enemiesFile .= "\t.db $".dechexpad2($data['sl1']*16 + $data['firedef']).",$".dechexpad2($data['sl2']*16 + $data['icedef']).",$".dechexpad2($data['sl3']*16 + $data['boltdef']).",$".dechexpad2($data['sl4']*16 + $data['bombdef']).",$".dechexpad2($data['sl5']*16 + ($data['boss']?0:1))."\n";
	$enemiesFile .= "\t.db ".$data['xpos'].",".$data['ypos'].",".$data['wait']."\n";
	$idCounter++;
}

$largeSprites = $sql->query("SELECT `name`,`data` FROM `bigSprites` WHERE 1");
$file2 = '';
$file2header = "BigSprites_Offpage_Sizes:\n";
foreach($largeSprites as $e){
	if($e['name']!=''){
		$defines['bigsprite_'.$e['name']] = $idCounter;
	}
	$defines['bigsprite_'.$idCounter] = $idCounter;
	
	$largeSpritesHeader .= "\t.dw EnemySprites_".$idCounter."_Size\n";
	
	$data = json_decode($e['data'],true);
	if($idCounter >= $bigSpritesPerPage){
		$size = strlen($data['frontBuf']) + 2; // no need for backbuf as they are hex-encoded anyways so we already have twice the size, +2 for the dimensions
		if($size > 828){
			$file2header .= '.fail "sprite too large, prod sorunome"'."\n";
		}
		$file2header .= "\t.dw $size\n";
		getBigSpriteData($data,$file2,$idCounter);
	}else{
		getBigSpriteData($data,$file,$idCounter);
	}
	$idCounter++;
}
$defines['bigsprites_pageswitch'] = $bigSpritesPerPage;

$file = $largeSpritesHeader.$file;
$file2 = $file2header.$file2;

file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/largeSprites.asm',$file);
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/largeSprites2.asm',$file2);

$html .= $file.'</textarea><h1>Enemies</h1><textarea style="width:100%;height:500px;">';

file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/enemies.asm',$enemiesFile);

$html .= $enemiesFile.'</textarea><h1>Areas</h1><textarea style="width:100%;height:500px;">';
$file = "const uint8_t area_enemies[] = {\n";
$areasLUT = [];
$idCounter = 0;
foreach($sql->query("SELECT `id`,`name`,`enemies` FROM `areas` WHERE 1") as $a){
	if($a['id'] !== NULL){
		$areasLUT[(int)$a['id']] = $idCounter;
		$defines['area_'.$a['id']] = $idCounter;
		if($a['name'] != ''){
			$defines['area_'.$a['name']] = $idCounter;
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
			$file .= "\n";
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
$blocknum = 0;
$blockLUT = "const uint8_t* const _Text_decompression_LUT[] = {\n";

$compress_string = function($min) use (&$file,&$hexData,&$uncompressedSize,&$compressedSize,&$bytesCompressed,&$blocknum,&$blockLUT,&$file2,&$textBlocksPerPage){
	$workData = [];
	$j = 2047;
	for($i = 0;$i < $j; $i++){
		if(!isset($hexData[$i])){
			break;
		}
		$workData[] = $hexData[$i];
		if($hexData[$i] == 'FREEZEBLOCK' || $hexData[$i] == 'UNFREEZEBLOCK'){
			$j++;
		}
		unset($hexData[$i]);
	}
	//$chunks = array_chunk($hexData,827 + $min);
	//echo "precompress length:";
	//var_dump(sizeof($workData));
	
	$chunk_prepend = [];
	if(($i = array_search('FREEZEBLOCK',array_reverse($workData,true))) !== false){
		
		$j = sizeof($workData);
		if(sizeof($hexData) > 0 && array_search('UNFREEZEBLOCK',array_slice($workData,$i)) === false){ // uho, we'll better flip this over
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
	
	
	$hexData = array_merge($chunk_prepend,$hexData);
	
	
	$uncompressedSize += sizeof($workData);
	//echo "compress length:";
	//var_dump(sizeof($workData));
	$workData[] = '0x84'; // end of chunk
	
	$hexDataOut = compress($workData);
	$compressedSize += sizeof($hexDataOut);
	$bytesCompressed += sizeof($workData) - sizeof($hexDataOut) + 5; // just appriximate the # of strings
	$f = "const uint8_t _Text_block_${blocknum}[] = {\n\t";
	for($i = 0;$i < sizeof($hexDataOut);$i++){
		$f .= $hexDataOut[$i].',';
	}
	$f .= "\n};\n";
	$blockLUT .= "\t_Text_block_$blocknum,\n";
	$blocknum++;
	$file .= $f;
};
$defines['first_offpage_textblock'] = $textBlocksPerPage;
$stringLUT = "const Strings_LUT stringsLut[] = {\n";
$strings = $sql->query("SELECT `name`,`string`,`compress` FROM `strings` WHERE `compress`=1");
$stringCounter = 0;
foreach($strings as $s) {
	$defines['string_'.$s['name']] = $stringCounter;
	$stringCounter++;
	$arrayCount = array_count_values($hexData);
	
	$min = $arrayCount['FREEZEBLOCK']??0;
	$min += $arrayCount['UNFREEZEBLOCK']??0;
	//echo "\n======\n",$s['name']."\n";
	//var_dump(sizeof($hexData));
	//var_dump($min);
	//echo "-----\n";
	$stringLUT .= "\t{ ".(sizeof($hexData) - $min).", $blocknum },\n";
	
	$bs = '';
	$s = getTextASM($s['string']);
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
	$hexData = array_merge($hexData, explode(',', rtrim($bs, ',')));
	
	
	$arrayCount = array_count_values($hexData);
	
	$min = $arrayCount['FREEZEBLOCK']??0;
	$min += $arrayCount['UNFREEZEBLOCK']??0;
	
	//echo "data length:";
	//var_dump(sizeof($hexData) - $min);
	
	
	while ((sizeof($hexData) - $min) >= 2047) { // time to compress this, but first look that everything after our flag is in the same block
		$compress_string($min);
	}
}
while (sizeof($hexData)) {
	$compress_string(0);
}

$defines['total_strings'] = $stringCounter;
$blockLUT .= "};\n";
$stringLUT .= "};\n";
$file .= "\n$blockLUT\n$stringLUT";
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
	
	$defines['world_'.$name] = $worldASMId;
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
		
		
		$defines['tilemap_'.$t['id']] = "0x".dechexpad2($mapId);
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
$defines['script_walk'] = 0;
$defines['script_action'] = 1;
$scriptfile = '';
$scriptfileLut = "const uint8_t* scripts[] = {\n";
$scriptId = 0;

foreach ($sql->query("SELECT `id`, `name`, `code` FROM `scripts`") as $s) {
	$s['code'] .= "\nreturn(true)";
	$defines['script_'.$s['name']] = $scriptId;
	$defines['script_'.(string)$s['id']] = $scriptId;
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
echo $page->getPage('Create Asm',$html,$lang,$pathPartsParsed);
