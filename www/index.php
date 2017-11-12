<?php
if(!$security->isLoggedIn() || !($user_info['power']&32)){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$sql->switchDb('soru_reuben3_meta');
include_once(realpath(dirname(__FILE__)).'/functions.php');
$html = '<h1>Reuben3-Meta development tool</h1>'.
	'<a href="sprites">Sprite editor</a><br>'.
	'<a href="spritesheet">Spritesheet editor</a><br>'.
	'<a href="tilemaps">Tilemap editor</a><br>'.
	'<a href="enemy">Enemy editor</a><br>'.
	'<a href="bigSprites">Big Sprites editor</a><br>'.
	'<a href="chars">Chars editor</a><br>'.
	'<a href="text">Text editor</a><br>'.
	'<a href="areas">Areas editor</a><br>'.
	'<a href="create">Create ASM</a>';
$sql->switchDb('soru_homepage');
echo $page->getPage('Reuben 3 dev tool',$html,$lang,$pathPartsParsed);
?>
