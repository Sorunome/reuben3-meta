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
	$s = $sql->query("SELECT `data`,`name` FROM `bigSprites` WHERE `id`=%d",[(int)$_GET['edit']],0);
	$html = getBigSpriteEditor('bigSprites',$s['data'],$s['name']);
	
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Enemies',$html,$lang,$pathPartsParsed);
}elseif(isset($_GET['save'])){
	$sql->query("UPDATE `bigSprites` SET `data`='%s',`name`='%s' WHERE `id`=%d",[$_POST['json'],$_POST['name'],(int)$_GET['save']]);
	echo 'Saved';
}elseif(isset($_GET['delete'])){
	$sql->query("DELETE FROM `bigSprites` WHERE `id`=%d",[(int)$_GET['delete']]);
}elseif(isset($_GET['new'])){
	$defData = [
		'frontBuf' => '0',
		'backBuf' => '0',
		'width' => 2,
		'height' => 16
	];
	$sql->query("INSERT INTO `bigSprites` (`data`) VALUES ('%s')",[json_encode($defData)]);
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("bigSprites");</script>Redirecting...',$lang,$pathPartsParsed);
}else{
	$html = '';
	
	$enemies = $sql->query("SELECT `id`,`name` FROM `bigSprites` WHERE 1");
	foreach($enemies as $e){
		$html .= '<a href="bigSprites?edit='.$e['id'].'">'.($e['name']!=''?$e['name']:$e['id']).'</a> ';
	}
	$html .='<hr>
	<a href="bigSprites?new">New</a><br>
	<a href=".">&lt;&lt; Back</a>';
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Big sprites',$html,$lang,$pathPartsParsed);
}
?>
