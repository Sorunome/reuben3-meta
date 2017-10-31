<?php
if((!$security->isLoggedIn() || !($user_info['power']&32)) && $_GET['pwd']!='vohphohDu0iekie7uc5leis2yohpha7KeebeaShiepei8airee'){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$sql->switchDb('soru_reuben3_meta');

if(isset($_GET['edit'])){
	$a = $sql->query("SELECT `name`,`id`,`enemies` FROM `areas` WHERE `id`=%d",[(int)$_GET['edit']],0);
	$e = $sql->query("SELECT `name`,`id` FROM `enemies` WHERE 1");
	$html = '
	<script type="text/javascript">
		$(function(){
			var all_enemies = '.json_encode($e).',
				enemies = '.json_encode(json_decode($a['enemies'])).',
				name = '.json_encode($a['name']?$a['name']:'').';
			$("#editstuff").append(
				"Name:",$("<input>").attr("type","text").val(name).change(function(){name = this.value;}),"<br>",
				$.map([0,1,2,3,4,5,6,7,8,9],function(i){
					return [
						"Enemy Slot ",i,": ",$("<select>").append(
							$("<option>").val(255).text("--none--").attr((255 == enemies[i]?"selected":"false"),"selected"),
							$.map(all_enemies,function(e){
								return $("<option>").val(e.id).text(e.name).attr((e.id == enemies[i]?"selected":"false"),"selected");
							})
						).change(function(){enemies[i] = parseInt(this.value,10);}),
						"<br>"
					];
				})
				
			);
			$("#save").click(function(e){
				homepage.post("areas?save='.(int)$_GET['edit'].'",{
						"json":JSON.stringify(enemies),
						"name":name
					},function(data){
						alert(data);
					});
			});
			$("#delete").click(function(e){
				e.preventDefault();
				if(confirm("are you sure you want to delete this area?")){
					homepage.get("areas?delete='.(int)$_GET['edit'].'",function(data){
						getPageJSON("areas");
					});
				}
			});
		});
	</script>
	Area ID: '.$a['id'].'<br>
	<div id="editstuff"></div>
	<button id="save">Save</button><button id="delete" style="float:right;">Delete</button>
	';
	
	
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Areas',$html,$lang,$pathPartsParsed);
}elseif(isset($_GET['save'])){
	$sql->query("UPDATE `areas` SET `enemies`='%s',`name`='%s' WHERE `id`=%d",[$_POST['json'],$_POST['name'],(int)$_GET['save']]);
	echo "Saved!";
}elseif(isset($_GET['delete'])){
	$sql->query("DELETE FROM `areas` WHERE `id`=%d",[(int)$_GET['delete']]);
}elseif(isset($_GET['new'])){
	$defData = [0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF];
	$sql->query("INSERT INTO `areas` (`enemies`) VALUES ('%s')",[json_encode($defData)]);
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("areas");</script>Redirecting...',$lang,$pathPartsParsed);
}else{
	$html = '';
	$areas = $sql->query("SELECT `id`,`name` FROM `areas` WHERE 1");
	foreach($areas as $a){
		$html .= '<a href="areas?edit='.$a['id'].'">'.($a['name']!=''?$a['name']:$a['id']).'</a> ';
	}
	$html .= '<hr>
	<a href="areas?new">New</a><br>
	<a href=".">&lt;&lt; Back</a>';
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Areas',$html,$lang,$pathPartsParsed);
}
?>
