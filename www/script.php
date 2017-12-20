<?php

if (!function_exists('dechexpad')) {
function dechexpad($i, $size){
	return str_pad(dechex($i),$size,'0',STR_PAD_LEFT);
}
}

class Parser{
	private $bytes = 0;
	private $variables = [];
	private $offset = 0;
	private $out = '';
	private $functions = [];
	private $if_stack = [];
	private $while_stack = [];
	private $defines = [];
	private $extra_vars = ['camera_x', 'camera_y', 'script_trigger', 'armor', 'wait', 'sword', 'tradequest', 'fright', 'tmp_map'];
	private $firstPass = true;
	private $labels = [];
	private $genericLabelCounter = 0;
	private $addressPrefix = '';
	private function getLabel(){
		return '++generic_label_'.($this->genericLabelCounter++).'++';
	}
	private function addLabel($label,$offset = 0){
		if(!$this->firstPass){
			return;
		}
		$this->labels[$this->addressPrefix.$label] = $this->bytes + $offset + $this->offset;
	}
	private function getAddress($label){
		if($this->firstPass){
			return '00000000';
		}
		return dechexpad($this->labels[$this->addressPrefix.$label], 8);
	}
	private function getVarNum($var){
		if (!$var) { // why the fuck is this needed
			return false;
		}
		if (isset($this->variables[$var])) {
			return dechexpad($this->variables[$var], 2);
		}
		if (($i = array_search($var, $this->extra_vars)) !== false) {
			return dechexpad(0xFF - $i, 2);
		}
		return false;
	}
	private function getVar($var){
		if(($s = $this->getVarNum($var))){
			return '80'.$s;
		}
		$var = (int)$var;
		if($var < 0){
			$var += 256;
		}
		if($var == 0x80){
			return '8080';
		}
		return dechexpad($var, 2);
	}
	private function getVar16($var) {
		if (strpos($var, '|') !== false) {
			$vars = explode('|', $var);
			return $this->getVar($vars[0]).$this->getVar($vars[1]);
		}
		$var = (int)$var;
		$upper = $var >> 8;
		$lower = $var & 0xFF;
		return $this->getVar($lower).$this->getVar($upper);
	}
	private function parseLine($line,$convertToBytes = true){
		global $defines;
		$out = '';
		$line = trim($line);
		$matches = [];
		if(!preg_match('/^(\\w+)(\\((?:[^()]*(?:\\([^)]*\\))?)*\\))?;?$/',$line,$matches)){
			return '';
		}
		$function = $matches[1];
		$args = [];
		if(isset($matches[2])){
			preg_match_all('/(?:\\(|,)([^(),]*(?:\\([^)]+[^),]*\\))?)/',$matches[2],$matches,PREG_SET_ORDER);
			foreach($matches as $m){
				if (isset($this->functions[$function]) && isset($this->functions[$function]['unparsed_args']) && $this->functions[$function]['unparsed_args']) {
				} else if (isset($this->functions[$function]) && isset($this->functions[$function]['unparsed_word_args']) && $this->functions[$function]['unparsed_word_args'] && preg_match('/^\w+$/', $m[1])){
				} else {
					$m[1] = strtr(trim($m[1]), $this->defines);
					if (preg_match('/^[\d\s*+\\/\\-()a-eA-Exo ]+$/', $m[1]) && !preg_match('/^[a-eA-Exo]+$/', $m[1])) {
						$m[1] = eval("return $m[1];");
					}
				}
				$args[] = trim($m[1]);
			}
		}
		if(isset($this->functions[$function])){
			$fn = $this->functions[$function];
			$numArgs = sizeof($args);
			if($numArgs >= $fn['args_min'] && $numArgs <= $fn['args_max']){
				$s = $fn['fn']($args);
				if($convertToBytes){
					$s = hex2bin($s);
				}
				$out .= $s;
			}
		}
		return $out;
	}
	private function saveVarNumber() {
		global $defines;
		if (!isset($defines['script_num_vars'])) {
			$defines['script_num_vars'] = 1;
		}
		if(sizeof($this->variables) > $defines['script_num_vars']){
			$defines['script_num_vars'] = sizeof($this->variables);
		}
	}
	public function __construct(){
		$this->functions = [
			'fade_to_white' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function() {
					return '01';
				}
			],
			'fade_from_white' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function() {
					return '02';
				}
			],
			'transition_map' => [
				'args_min' => 3,
				'args_max' => 4,
				'fn' => function($args) {
					$s = '';
					if (sizeof($args) == 4) {
						$s = '03';
					} else {
						$s = '04';
					}
					foreach($args as $a) {
						$s .= $this->getVar($a);
					}
					return $s;
				}
			],
			'focus_cam' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function() {
					return '06';
				}
			],
			'fade_home' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function() {
					return '07';
				}
			],
			'set_var' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args) {
					$s = '08'.$this->getVarNum($args[0]).$this->getVar($args[1]);
					return '08'.$this->getVarNum($args[0]).$this->getVar($args[1]);
				}
			],
			'set_var16' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args) {
					$num = $this->getVarNum($args[0]);
					$num2 = dechex($num)+1;
					$this->saveVarNumber();
					global $defines;
					if ($num2 > $defines['script_num_vars']) {
						$defines['script_num_vars'] = $num2;
					}
					$num2 = dechexpad2($num2);
					$i = $this->getVar16($args[1]);
					return '08'.$num.$i[0].$i[1].'08'.$num2.$i[2].$i[3];
				}
			],
			'label' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					$this->addLabel($args[0]);
				}
			],
			'goto' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					return '09'.$this->getAddress($args);
				}
			],
			'jump_ifnot' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args) {
					return '0a';
				}
			],
			'lt' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args) {
					return '0b'.$this->getVar($args[0]).$this->getVar($args[1]);
				}
			],
			'gt' => [ // gt is like lt....with reversed arguments!
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args) {
					return '0b'.$this->getVar($args[1]).$this->getVar($args[0]);
				}
			],
			'add' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args) {
					return '0c'.$this->getVarNum($args[0]).$this->getVar($args[1]);
				}
			],
			'inc' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					return '0e'.$this->getVarNum($args[0]);
				}
			],
			'dec' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					return '0f'.$this->getVarNum($args[0]);
				}
			],
			'call' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					return '10'.$this->getVar($args[0]);
				}
			],
			'move_player' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args) {
					return '11'.$this->getVar($args[0]).$this->getVar($args[1]);
				}
			],
			'is_event' => [
				'args_min' => 1,
				'args_max' => 1,
				'unparsed_word_args' => true,
				'fn' => function($args) {
					return '12'.(isset($this->defines['event_'.$args[0]])?dechexpad($this->defines['event_'.$args[0]], 2):$this->getVar($args[0]));
				}
			],
			'set_event' => [
				'args_min' => 1,
				'args_max' => 1,
				'unparsed_word_args' => true,
				'fn' => function($args) {
					return '13'.(isset($this->defines['event_'.$args[0]])?dechexpad($this->defines['event_'.$args[0]], 2):$this->getVar($args[0]));
				}
			],
			'clear_event' => [
				'args_min' => 1,
				'args_max' => 1,
				'unparsed_word_args' => true,
				'fn' => function($args) {
					return '14'.(isset($this->defines['event_'.$args[0]])?dechexpad($this->defines['event_'.$args[0]], 2):$this->getVar($args[0]));
				}
			],
			'eq' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args) {
					return '15'.$this->getVar($args[0]).$this->getVar($args[1]);
				}
			],
			'text' => [
				'args_min' => 1,
				'args_max' => 2,
				'unparsed_word_args' => true,
				'fn' => function($args) {
					$i = $this->defines['string_'.$args[0]] ?? $args[0];
					$i = $this->getVar16($i);
					if (sizeof($args) == 1) {
						return '16'.$i;
					}
					return '17'.$i.$this->getVarNum($args[1]);
				}
			],
			'shake_screen' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args) {
					return '18';
				}
			],
			'update_screen' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args) {
					return '19';
				}
			],
			'set_tile' => [
				'args_min' => 3,
				'args_max' => 3,
				'fn' => function($args) {
					$i = $args[2];
					if (isset($this->defines['sprite_'.$args[2]])) {
						$i = hexdec($this->defines['sprite_'.$args[2]]);
					}
					$i = $this->getVar16($i);

					return '1A'.$this->getVar($args[0]).$this->getVar($args[1]).$i;
				}
			],
			'fade_map' => [
				'args_min' => 1,
				'args_max' => 4,
				'fn' => function($args) {
					$s = '';
					if (sizeof($args) == 4) {
						$s = '22';
					} else if (sizeof($args) == 2) {
						$s = '1B';
					} else {
						$s = '1C';
					}
					foreach($args as $a) {
						$s .= $this->getVar($a);
					}
					return $s;
				}
			],
			'get_player_x' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					return '1D'.$this->getVarNum($args[0]);
				}
			],
			'get_player_y' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					return '1E'.$this->getVarNum($args[0]);
				}
			],
			'render' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args) {
					return '1F';
				}
			],
			'draw_tile' => [
				'args_min' => 3,
				'args_max' => 3,
				'fn' => function($args) {
					$i = $args[2];
					if (isset($this->defines['sprite_'.$args[2]])) {
						$i = hexdec($this->defines['sprite_'.$args[2]]);
					}
					$i = $this->getVar16($i);
					return '20'.$this->getVar($args[0]).$this->getVar($args[1]).$i;
				}
			],
			'reload_map' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args) {
					return '21';
				}
			],
			'add_gold' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					return '23'.$this->getVar16($args[0]);
				}
			],
			'shop' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args) {
					$i = $this->defines['string_'.$args[0]] ?? $args[0];
					$i = $this->getVar16($i);
					return '24'.$i.$this->getVar16($args[1]);
				}
			],
			'shop_bottle' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args) {
					$i = $this->defines['string_'.$args[0]] ?? $args[0];
					$i = $this->getVar16($i);
					return '25'.$i.$this->getVar16($args[1]);
				}
			],
			'set_bottle' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					$i = $this->defines['bottle_content_'.$args[0]] ?? $args[0];
					$i = $this->getVar($i);
					return '26'.$i;
				}
			],
			'add_bombs' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					return '27'.$this->getVar($args[0]);
				}
			],
			'get_swimsuit' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args) {
					return '28';
				}
			],
			'transition_portal' => [
				'args_min' => 6,
				'args_max' => 6,
				'fn' => function($args) {
					$s = '29';
					foreach($args as $a) {
						$s .= $this->getVar($a);
					}
					return $s;
				}
			],
			'move_left_and_shake_screen' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args) {
					return '2A';
				}
			],

			'add_enemy' => [
				'args_min' => 3,
				'args_max' => 3,
				'fn' => function($args) {
					return '05'.$this->getVar($args[0]).$this->getVar($args[1]).$this->getVar($args[2]);
				}
			],
			'hex' => [
				'args_min' => 1,
				'args_max' => 1,
				'unparsed_args' => true,
				'fn' => function($args) {
					return $args[0];
				}
			],
			'put_address' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					return $this->getAddress($args[0]);
				}
			],
			'set_address_prefix' => [
				'args_min' => 0,
				'args_max' => 1,
				'fn' => function($args) {
					if (sizeof($args) == 0) {
						$this->addressPrefix = '';
					} else {
						$this->addressPrefix = $args[0];
					}
				}
			],

			'return_false' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args) {
					return 'fe';
				}
			],
			'return_true' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args) {
					return 'ff';
				}
			],

			'define_vars' => [
				'args_min' => 1,
				'args_max' => 10,
				'fn' => function($args) {
					foreach($args as $i => $a) {
						$this->variables[$a] = $i;
					}
				}
			],
			'clear_vars' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function() {
					$this->saveVarNumber();
					$this->variables = [];
				}
			],
			'return' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					if ($args[0] == 'true') {
						return 'ff';
					}
					return 'fe';
				}
			],
			'if' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					$out = '0a'.$this->parseLine($args[0],false);
					$label = $this->getLabel();
					$this->if_stack[] = $label;
					return $out.$this->getAddress($label);
				}
			],
			'ifnot' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					$out = '0D'.$this->parseLine($args[0],false);
					$label = $this->getLabel();
					$this->if_stack[] = $label;
					return $out.$this->getAddress($label);
				}
			],
			'else' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function() {
					$label = array_pop($this->if_stack);
					$this->addLabel($label,5);

					$out = '09';
					$label = $this->getLabel();
					$this->if_stack[] = $label;
					return $out.$this->getAddress($label);
				}
			],
			'endif' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function() {
					$label = array_pop($this->if_stack);
					$this->addLabel($label);
				}
			],
			'do' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function() {
					$label = $this->getLabel();
					$this->addLabel($label);
					$this->while_stack[] = $label;
				}
			],
			'while' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args) {
					$out = '0d'.$this->parseLine($args[0],false);
					$label = array_pop($this->while_stack);
					$out .= $this->getAddress($label);
					return $out;
				}
			]
		];
	}
	public function parse($script,$offset = 0,$def = []){
		$this->defines = $def;
		$this->offset = $offset;
		$this->variables = [];
		$this->bytes = 0;
		$out = '';
		$this->if_stack = [];
		$this->while_stack = [];
		$this->labels = [];
		$this->firstPass = true;
		$this->genericLabelCounter = 0;
		$this->addressPrefix = '';
		foreach(explode("\n",strtolower($script)) as $line){
			$out .= $this->parseLine($line);
			$this->bytes = strlen($out);
		}
		$this->addressPrefix = '';
		$this->bytes = 0;
		$out = '';
		$this->firstPass = false;
		$this->genericLabelCounter = 0;
		foreach(explode("\n",strtolower($script)) as $line){
			$out .= $this->parseLine($line);
			$this->bytes = strlen($out);
		}
		$this->saveVarNumber();
		return $out;
	}
}
$parser = new Parser();
