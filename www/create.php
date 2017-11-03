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
function parseCode($s){
	$s = preg_replace('/setEvent\(([\d\w-]+)\)/','setEvent_real(event_${1}_offset,event_${1}_bit)',$s);
	$s = preg_replace('/isEvent\(([\d\w-]+)\)/','isEvent_real(event_${1}_offset,event_${1}_bit)',$s);
	$s = preg_replace('/clearEvent\(([\d\w-]+)\)/','clearEvent_real(event_${1}_offset,event_${1}_bit)',$s);
	$s = preg_replace('/toggleEvent\(([\d\w-]+)\)/','toggleEvent_real(event_${1}_offset,event_${1}_bit)',$s);
	$s = preg_replace('/doSpeech\(([\d\w-]+)\)/','doSpeech_real(string_${1})',$s);
	$s = preg_replace('/doBattle\(([\d\w-]+)\)/','doBattle_real(enemy_${1})',$s);
	$s = preg_replace('/shop\(([\d\w-]+),([\d\w-]+)\)/','shop_real(string_${1},${2},0)',$s);
	$s = preg_replace('/shop_bottle\(([\d\w-]+),([\d\w-]+)\)/','shop_real(string_${1},${2},1)',$s);
	
	$s = preg_replace('/call ([\d\w-]+)\s+ret\s*$/','jp ${1}',$s);
	return $s;
}
function getTextASM($s){
	$s = str_replace("\n\n\n","\t",$s);
	$s = str_replace("\n\n","\x01",$s);
	$s = str_replace("\n","\x02",$s);
	$s .= "\x04";
	if(mb_strpos($s,"{#ask}\x02") !== false){
		$s = str_replace("{#ask}\x02","\x03",$s);
		$i = mb_strpos($s,"\x03")+1;
		$ss = explode(' ',mb_substr($s,$i));
		$s = str_replace('_',' ',$s);
		$i = 0;
		$s .= "\x05".sizeof($ss).',';
		foreach($ss as $si){
			$s .= (string)($i*6).',';
			$i += strlen($si)+1; // +1 because of the space
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
	for($i = 0;$i < mb_strlen($s);$i++){
		$c = mb_substr($s,$i,1);
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
$idCounter = 11;
$charsLUT = [
	'0' => 1,
	'1' => 2,
	'2' => 3,
	'3' => 4,
	'4' => 5,
	'5' => 6,
	'6' => 7,
	'7' => 8,
	'8' => 9,
	'9' => 10,
	"\x05" => "",
	"\x04" => 0,
	"\x03" => 0xFC,
	"\t" => 0xFD,
	"\x01" => 0xFE,
	"\x02" => 0xFF
];
foreach($charsInUse as $c){
	if(mb_strpos("0123456789\n\t\x01\x02\x03\x04\x05",$c)===false){
		$charsLUT[$c] = $idCounter;
		//$sql->query("UPDATE `font` SET `asm_id`=%d WHERE `char`='%s'",[$idCounter,$c]);
		$idCounter++;
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


$file = "const uint16_t sprites_data[] = {\n\t8, 8,\n";
$spriteData = [];
foreach($sql->query("SELECT `buffer1`,`buffer2`,`name`,`id` FROM `sprites` WHERE `id` IN (".implode(',',array_map('intval',$spritesWithId)).")") as $s){
	if($s['name']){
		$defines['sprite_'.$s['name']] = '0x'.$spritesLUT[$s['id']];
	}
	$out = "";
	for($i = 0;$i < 16;$i += 2){
		$b1 = hex2binstr(substr($s['buffer1'],$i,2));
		$b2 = hex2binstr(substr($s['buffer2'],$i,2));
		for ($j = 0; $j < 8; $j++) {
			$n = ($b1[$j] == '1')*2 + ($b2[$j] == '1')*1;
			$out .= ['0xFFFF', '0xACD0', '0x72C7', '0x0000'][$n].',';
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
$html .= $file.'</textarea><h1>Tilemaps</h1><textarea style="width:100%;height:500px;">';

foreach($sql->query("SELECT `offset`,`bit`,`id`,`name` FROM `events`") as $e){
	$defines['event_'.$e['id'].'_offset'] = $e['offset'];
	$defines['event_'.$e['id'].'_bit'] = $e['bit'];
	if($e['name'] != ''){
		$defines['event_'.$e['name'].'_offset'] = $e['offset'];
		$defines['event_'.$e['name'].'_bit'] = $e['bit'];
	}
}

$eventTilesLutLut = '';
$eventTilesLut = [];
$eventTilesCode = '';
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
	$worldLUT .= "/*dynTilesStart_$worldASMId*/ 0, ";
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
		$dynTiles = $sql->query("SELECT t1.`id`,t1.`x`,t1.`y`,t1.`newTile`,t2.`offset`,t2.`bit` FROM `dynTiles` AS t1 INNER JOIN `events` AS t2 ON t1.`event`=t2.`id` WHERE t1.`refId`=%d",[$t['id']]);
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
				if($te['add_jump']){
					$te['code'] .= "\nret";
				}
				$eventTilesCode .= "event_tile_routine_".$te['id'].":\n\t".parseCode(str_replace("\n","\n\t",$te['code']))."\n";
				$eventTilesCode .= "\n";
			}
			$eventTilesLut['event_tiles_LUT_for_map_'.$t['id']] = $s;
		}
		if($dynTiles[0]['id'] !== NULL){
			if(!$dyntilesOnWorld){
				$dynTilesCode .= "dynTilesStart_$worldASMId:\n";
				$dyntilesOnWorld = true;
			}
			foreach($dynTiles as $d){
				$dynTilesCode .= "\t.db $".dechexpad2($mapId).",".$d['offset'].",".(2**($d['bit'])).",".($d['y']*12 + $d['x']).",$".$spritesLUT[$d['newTile']]."\n";
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
		$dynTilesCode .= "\t.db \$ff\n";
	} else {
		$defines['dynTilesStart_'.$worldASMId] = 0;
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

$file .= "\n\n\n";
$file .= $worldLUT;
$file .= "\n\n\n";
//$file .= $dynTilesCode;
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/maps.h',$file);


$defines['tilemaps_header_size'] = '2';
$defines['tilemaps_width'] = $mapDimensions['width'];
//$defaultMap = $sql->query("SELECT `asm_id` FROM `tilemaps` WHERE `id`=%d",[$vars->get('defaultMap')],0);
//$defines['tilemaps_defaultmap'] = "$".$defaultMap['asm_id'];


$compressedPercent = (($compressedSize/$uncompressedSize)*100);
$html .= $file.'</textarea>Compressed: '.$compressedSize.'/'.$uncompressedSize.' ('.$compressedPercent.'%)<br>Bytes compressed: '.$bytesCompressed;


$html .= '<h1>Font Data</h1><textarea style="width:100%;height:500px;">';

$sqlquery = "SELECT `char`,`bighex`,`smallhex` FROM `font` WHERE `char` IN (";
foreach($charsInUse as $c){
	$sqlquery .= "'%s',";
}
$chars = $sql->query(rtrim($sqlquery,',').')',$charsInUse);
$charsbig = [];
$charssmall = [];
foreach($chars as $c){
	$bighex = "\t.db ";
	for($i = 0;$i < 14;$i += 2){
		$bighex .= "$".substr($c['bighex'],$i,2).",";
	}
	$charsbig[$c['char']] = rtrim($bighex,',')."\n";
	
	
	$smallhex = "\t.db ";
	for($i = 0;$i < 10;$i += 2){
		$smallhex .= "$".substr($c['smallhex'],$i,2).",";
	}
	$charssmall[$c['char']] = rtrim($smallhex,',')."\n";
}
asort($charsLUT);
$bighex = '';
$smallhex = '';
foreach($charsLUT as $id => $asmid){
	if(isset($charsbig[$id])){
		$bighex .= $charsbig[$id];
		$smallhex .= $charssmall[$id];
	}
}

$file = "Chars:\n".$bighex."\nSmallChars:\n".$smallhex;

file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/font.asm',$file);

$html .= $file.'</textarea><h1>String Data</h1><textarea style="width:100%;height:500px;">';
$strings = $sql->query("SELECT `name`,`string`,`compress` FROM `strings` WHERE 1");
$file = '';
$file2 = '';
$hexData = [];
$uncompressedSize = 0;
$compressedSize = 0;
$bytesCompressed = 0;
$blocknum = 0;
$blockLUT = "_Text_decompression_LUT:\n";

$compress_string = function($min) use (&$file,&$hexData,&$uncompressedSize,&$compressedSize,&$bytesCompressed,&$blocknum,&$blockLUT,&$file2,&$textBlocksPerPage){
	$workData = [];
	$j = 827;
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
	$workData[] = '$fb';
	
	$hexDataOut = compress($workData);
	$compressedSize += sizeof($hexDataOut);
	$bytesCompressed += sizeof($workData) - sizeof($hexDataOut) + 5; // just appriximate the # of strings
	$f = "_Text_block_$blocknum:\n";
	$f .= "\t.db ";
	for($i = 0;$i < sizeof($hexDataOut);$i++){
		$f .= $hexDataOut[$i].',';
	}
	$f = rtrim($f,',')."\n";
	$blockLUT .= "\t.dw _Text_block_$blocknum\n";
	$blocknum++;
	if($blocknum > $textBlocksPerPage){
		$file2 .= $f;
	}else{
		$file .= $f;
	}
};
$defines['first_offpage_textblock'] = $textBlocksPerPage;
$stringLUT = '';
foreach($strings as $s){
	if((int)$s['compress']){
		$stringLUT .= 'string_'.$s['name'].":\n";
		$arrayCount = array_count_values($hexData);
		
		$min = $arrayCount['FREEZEBLOCK']??0;
		$min += $arrayCount['UNFREEZEBLOCK']??0;
		//echo "\n======\n",$s['name']."\n";
		//var_dump(sizeof($hexData));
		//var_dump($min);
		//echo "-----\n";
		$stringLUT .= "\t.dw ".(sizeof($hexData) - $min)."\n";
		$stringLUT .= "\t.db $blocknum\n";
		
		$bs = '';
		$s = getTextASM($s['string']);
		for($i = 0;$i < mb_strlen($s);$i++){
			$c = mb_substr($s,$i,1);
			//if($i == 0){
			//	echo $c."\n";
			//}
			if($c == "\x05"){
				$bs .= 'FREEZEBLOCK,'.mb_substr($s,$i+1).'UNFREEZEBLOCK,';
				break;
			}
			$bs .= (string)$charsLUT[$c].',';
		}
		//echo "new length:";
		//var_dump(sizeof(explode(',',rtrim($bs,','))));
		$hexData = array_merge($hexData,explode(',',rtrim($bs,',')));
		
		
		$arrayCount = array_count_values($hexData);
		
		$min = $arrayCount['FREEZEBLOCK']??0;
		$min += $arrayCount['UNFREEZEBLOCK']??0;
		
		//echo "data length:";
		//var_dump(sizeof($hexData) - $min);
		
		
		while((sizeof($hexData) - $min) >= 827) { // time to compress this, but first look that everything after our flag is in the same block
			$compress_string($min);
		}
	} else {
		$file .= "string_$s[name]:\n\t.db ";
		$s = getTextASM($s['string']);
		for($i = 0;$i < mb_strlen($s);$i++){
			$c = mb_substr($s,$i,1);
			if($c == "\x05"){
				$file .= mb_substr($s,$i+1);
				break;
			}
			$file .= (string)$charsLUT[$c].',';
		}
		$file = rtrim($file,',')."\n";
	}
}
while(sizeof($hexData)){
	$compress_string(0);
}
$file .= "\n$blockLUT\n$stringLUT";
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/strings.asm',$file);
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/strings2.asm',$file2);

$compressedPercent = (($compressedSize/$uncompressedSize)*100);
$html .= $file.'</textarea>Compressed: '.$compressedSize.'/'.$uncompressedSize.' ('.$compressedPercent.'%)<br>Bytes compressed: '.$bytesCompressed;


$html .= '<h1>Defines</h1><textarea style="width:100%;height:500px;">';

$file = "";
foreach($defines as $label => $value){
	$label = strtoupper($label);
	$file .= "#define $label $value\n";
}
file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/defines.h',$file);

$html .= $file.'</textarea><h1>Event Tiles</h1><textarea style="width:100%;height:500px;">';

$file = $eventTilesCode;

file_put_contents('/var/www/www.sorunome.de/reuben3-meta/out/eventtiles.asm',$file);


$html .= $file.'</textarea><hr><a href="/reuben3">&lt;&lt; Back</a>';

$sql->switchDb('soru_homepage');
echo $page->getPage('Create Asm',$html,$lang,$pathPartsParsed);
?>
