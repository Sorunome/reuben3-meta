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
$html = '';

if(isset($_GET['edit'])){
	$s = $sql->query("SELECT `name`,`string`,`id` FROM `strings` WHERE `id`=%d",[(int)$_GET['edit']],0);
	$html = '
	<script type="text/javascript">
		$(document).ready(function(){
			var data = '.json_encode(['name' => $s['name'],'string' => $s['string']]).';
			$("#container").append(
				$("<input>").val(data.name).change(function(){data.name = this.value;}),
				"<br><br>",
				$("<div>").css({position:"relative",width:0,height:0}).append($("<div>").css({position:"absolute",height:300,width:1,"border-right":"1px solid black",left:114,top:3})),
				$("<textarea>").css({width:400,height:300}).text(data.string).change(function(){data.string = this.value;})
			);
			$("#save").click(function(e){
				e.preventDefault();
				homepage.post("text?save='.$s['id'].'",{
					name:data.name,
					string:data.string
				},function(d){
					alert(d);
				});
			});
		});
	</script>
	Text ID: '.$s['id'].'<br>
	<div id="container"></div><br>
	<button id="save">Save</button><br><br>
	<a href="text">&lt;&lt; Back</a>';
}elseif(isset($_GET['save'])){
	$sql->query("UPDATE `strings` SET `name`='%s',`string`='%s' WHERE `id`=%d",[$_POST['name'],$_POST['string'],(int)$_GET['save']]);
	echo 'Saved!';
}elseif(isset($_GET['new'])){
	$sql->query("INSERT INTO `strings` () VALUES ()");
	$html = '<script type="text/javascript">getPageJSON("text?edit='.($sql->insertId()).'")</script>';
}else{
	$strings = $sql->query("SELECT `id`,`name` FROM `strings`");
	foreach($strings as $s){
		$html .= '<a href="text?edit='.$s['id'].'">'.$s['name'].'</a><br>';
	}
	$html .= '<br><br><a href=text?new>New</a><br><a href=".">&lt;&lt; Back</a>';
}
if($html != ''){
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Chars',$html,$lang,$pathPartsParsed);
}
?>
