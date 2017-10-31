<?php
if(!$security->isLoggedIn() || !($user_info['power']&32)){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$sql->switchDb('soru_reuben3_meta');
include_once(realpath(dirname(__FILE__)).'/functions.php');
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(E_ALL);

if(isset($_GET['edit'])){
	$e = $sql->query("SELECT `enemyData`,`name` FROM `enemies` WHERE `id`=%d",[(int)$_GET['edit']],0);
	$html = getBigSpriteEditor('enemy',$e['enemyData'],$e['name'],true);
	
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Enemies',$html,$lang,$pathPartsParsed);
}elseif(isset($_GET['save'])){
	$sql->query("UPDATE `enemies` SET `enemyData`='%s',`name`='%s' WHERE `id`=%d",[$_POST['json'],$_POST['name'],(int)$_GET['save']]);
	echo 'Saved';
}elseif(isset($_GET['delete'])){
	$sql->query("DELETE FROM `enemies` WHERE `id`=%d",[(int)$_GET['delete']]);
}elseif(isset($_GET['new'])){
	$defData = [
		'level' => 1,
		'hp' => 1,
		'exp' => 1,
		'sl1' => 0,
		'sl2' => 0,
		'sl3' => 0,
		'sl4' => 0,
		'sl5' => 0,
		'tile1' => 1,
		'tile2' => 1,
		'tile3' => 1,
		'tile4' => 1,
		'xpos' => 0,
		'ypos' => 0,
		'wait' => 0,
		'frontBuf' => '0',
		'backBuf' => '0',
		'width' => 2,
		'height' => 16,
		'firedef' => 1,
		'icedef' => 1,
		'boltdef' => 1,
		'bombdef' => 4,
		'boss' => false
	];
	$sql->query("INSERT INTO `enemies` (`enemyData`) VALUES ('%s')",[json_encode($defData)]);
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("enemy");</script>Redirecting...',$lang,$pathPartsParsed);
}else{
	$html = '';
	
	$enemies = $sql->query("SELECT `id`,`name` FROM `enemies` WHERE 1");
	foreach($enemies as $e){
		$html .= '<a href="enemy?edit='.$e['id'].'">'.($e['name']!=''?$e['name']:$e['id']).'</a> ';
	}
	$html .='<hr>
	<a href="enemy?new">New</a><br>
	<hr>
	<h1>Test stats</h1>
	<div style="display:inline-block;padding:10px;border:1px solid black;">
		<b>Reuben</b><br>
		Level: <input id="reuben_level" type="number" value="1"><br>
		Wait: <input id="reuben_wait" type="number" value="255"><br>
		HP: <input id="reuben_hp" type="number" value="0"><br>
		MP: <input id="reuben_mp" type="number" value="0"><br>
		Weapon: <select id="reuben_weapon">
			<option value="2">Stick (2)</option>
			<option value="4">Axe (4)</option>
			<option value="5">Sword (5)</option>
			<option value="6">Breadsword (6)</option>
			<option value="7">Poisoned Sword (7)</option>
			<option value="8">Holy Sword (8)</option>
		</select><br>
		Armor:<select id="reuben_armor">
			<option>1</option>
			<option>2</option>
			<option>3</option>
			<option>4</option>
			<option>5</option>
			<option>6</option>
			<option>7</option>
		</select>
	</div>
	<div style="display:inline-block;padding:10px;border:1px solid black;">
		<b>Enemy</b><br>
		Level: <input id="enemy_level" type="number" value="1"><br>
		Wait: <input id="enemy_wait" type="number" value="255"><br>
		HP: <input id="enemy_hp" type="number" value="50"><br>
		';
	for($i = 0;$i < 5;$i++){
		$html .= 'Slot '.$i.': <select id="enemy_slot'.$i.'" -data-id="'.$i.'">
			<option value="0">Normal</option>
			<option value="1">Fire</option>
			<option value="2">Bolt</option>
			<option value="3">Poison</option>
			<option value="4">Stun</option>
		</select><br>';
	}
	$html .= '
	</div><br>
	<button id="fight">Fight</button><br>
	<div id="fightResults"></div>
	<script type="text/javascript">
		$(function(){
			var reuben_level,reuben_maxhp,reuben_hp,reuben_maxmp,reuben_mp,reuben_weapon,reuben_armor,reuben_wait,
				enemy_level,enemy_hp,enemy_slots = [0,0,0,0,0],enemy_wait;
			$("#reuben_level").change(function(e){
				reuben_level = parseInt(this.value,10);
				reuben_maxhp = reuben_hp = reuben_level==1?120:120 + (10*Math.pow(reuben_level-1,2));
				$("#reuben_hp").val(reuben_hp);
			}).change();
			$("#reuben_hp").change(function(e){
				reuben_hp = parseInt(this.value,10);
			}).change();
			$("#reuben_wait").change(function(e){
				reuben_wait = parseInt(this.value,10);
			}).change();
			$("#reuben_mp").change(function(e){
				reuben_mp = parseInt(this.value,10);
			}).change();
			$("#reuben_weapon").change(function(e){
				reuben_weapon = parseInt(this.value,10);
			}).change();
			$("#reuben_armor").change(function(e){
				reuben_armor = parseInt(this.value,10);
			}).change();
			$("#enemy_level").change(function(e){
				enemy_level = parseInt(this.value,10);
			}).change();
			$("#enemy_wait").change(function(e){
				enemy_wait = parseInt(this.value,10);
			}).change();
			$("#enemy_hp").change(function(e){
				enemy_hp = parseInt(this.value,10);
			}).change();
			for(var i = 0;i < 5;i++){
				$("#enemy_slot"+i).change(function(e){
					enemy_slots[parseInt($(this).attr("-data-id"),10)] = parseInt(this.value,10);
				}).change();
			}
			$("#fight").click(function(e){
				$("#fightResults").text("Fighting...");
				var memAlert = false,
					poison = false,
					reuben_turns = 0,
					enemy_turns = 0,
					reuben_hp_battle = reuben_hp,
					enemy_hp_battle = enemy_hp,
					reuben_wait_battle = reuben_wait,
					enemy_wait_battle = enemy_wait;
				while(reuben_hp_battle > 0 && enemy_hp_battle > 0){
					if(reuben_wait_battle-- <= 0){
						var top = reuben_weapon*(2 + Math.floor(Math.random() * 10 * reuben_level + 1) + 2*Math.pow(reuben_level,2));
						if(top > 65535){
							memAlert = true;
						}
						enemy_hp_battle -= Math.round(top / (2*enemy_level));
						reuben_wait_battle = reuben_wait;
						reuben_turns++;
					}
					if(enemy_wait_battle-- <= 0){
						var attack = enemy_slots[Math.floor(Math.random()*5)];
						var top = (5 + Math.floor(Math.random() * 10 * enemy_level + 1) + Math.pow(enemy_level,3));
						if(top > 65535){
							memAlert = true;
						}
						var bottom = reuben_level + 4*(Math.floor(Math.random() * reuben_armor + 1) + reuben_armor) + Math.pow(reuben_armor,2);
						if(bottom > 65535){
							memAlert = true;
						}
						var factor = 0;
						switch(attack){
							case 0:
								factor = 1;
								break;
							case 1:
								factor = 2;
								break;
							case 2:
								factor = 4;
								break;
							case 3:
								factor = 0;
								poison = true;
								break;
							case 4:
								factor = 1; // enough to simulate stun right now
								reuben_wait_battle = reuben_wait;
								break;
							
						}
						reuben_hp_battle -= factor * 5 * Math.round(top / bottom);
						if(poison){
							reuben_hp_battle -= enemy_level;
						}
						enemy_wait_battle = enemy_wait;
						enemy_turns++;
					}
				}
				$("#fightResults").empty().append(
					$("<b>").text("Fight results"),
					"<br>Reuben HP: "+reuben_hp_battle,
					"<br>Reuben Turns: "+reuben_turns,
					"<br>Enemy HP: "+enemy_hp_battle,
					"<br>Enemy Turns: "+enemy_turns,
					(memAlert?[
						"<br>",$("<b>").text("2 bytes overflowed")
					]:"")
				);
			});
		});
	</script>
	<hr>
	<br>
	<a href=".">&lt;&lt; Back</a>';
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Enemies',$html,$lang,$pathPartsParsed);
}
?>
