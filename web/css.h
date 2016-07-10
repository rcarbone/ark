/*
 * css.h - html css without file for ark's plugins
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
 *                                 _
 *                       __ _ _ __| | __
 *                      / _` | '__| |/ /
 *                     | (_| | |  |   <
 *                      \__,_|_|  |_|\_\
 *
 * 'ark', is the home for asynchronous libevent-based plugins
 *
 *
 *                  (C) Copyright 2007-2008
 *         Rocco Carbone <rocco /at/ tecsiel /dot/ it>
 *
 * Released under the terms of GNU General Public License
 * at version 3;  see included COPYING file for details
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
 *
 */


#if !defined(_CSS_H_)
# define _CSS_H_

static unsigned char css [] =
{
  "body"
  "{"
  "margin:0px;"
  "padding:0px;"
  "margin-top:10px;"
  "font-family: verdana, sans-serif;"
  "font-size:11px;"
  "background-color:#2f292a;"
  "background-image: url(/image?png=background);"
  "color:#404041;"
  "}"
  "#page"
  "{position: relative;margin:0px;padding:0px;width:817px;left: 50%;margin-left: -414px;background-color:#f1eeed;}"
  "a {color:inherit;font-weight:inherit;font-size:inherit;text-decoration:none;}"
  "a.clean , p a.clean , .clean{text-decoration:none;}"
  "a img {text-decoration:none;}"
  "#banner"
  "{position:relative;left:-1px;width:817px;height:100px;}"
  ".homepage #banner{position:relative;left:-1px;width:817px;height:73px;}"
  ".homepage #caption{position:relative;left:-1px;width:817px;height:214px;}"
  "#caption{position:relative;left:-1px;width:817px;height:214px;}"
  ""
  "" /* Logo grande sulla homepage del progetto */
  "#projectlogo{position:absolute;top:120px;left:450px;}"
  "" /* Immagine title */
  "#imgtitle{position:absolute;top:10px;left:50px;}"
  "" /* Mascotte fluttuante sull'header */
  "#mascotte{position:absolute;top:10px;left:630px;}"
  ""
  "#menu {height:25px;background-repeat:repeat-x;text-align:center;font-size:11px;line-height:25px;"
  " background-image: url(/image?png=button);color:#404041;font-weight:bold;border-right:1px solid #8a6f37;"
  " border-left:1px solid #8a6f37;border-bottom:1px solid #404041;}"
  "#menu a {color:#404041;display:block;padding:0px 15px 0px 20px;width:.1em;border-right: 1px solid #000;"
  " white-space: nowrap;height:25px;}"
  "#menu > ul a { width:auto; }"
  "#menu a:hover{background-color:#b6753e;}"
  "#menu ul{margin:0px;padding:0px;list-style:none;}"
  "#menu li { float:left; }"
  "#menu.clean li {border:none;}"
  "#menu ri { float:right; }" /* Bottoni destra */
  "#menu.clean ri {border:none;}"
  "#menu.clean { background:transparent; border:none; height:15px; line-height:15px; }"
  "#menu.clean a { border-right: 1px solid #404041;height:15px; }"
  "#menu.clean a:hover { background:transparent;color:#404041;  }"
  "#menu li.active a { color:#3e1c0a; }"
  "#menu ri.active a { color:#3e1c0a; border-right:0px; border-left:1px solid #000;}"
  ""
  "#content{position:relative;clear:both;z-index:10;background-color:#f1eeed;border-right:1px solid #8a6f37;"
  " border-left:1px solid #8a6f37;width:815px;line-height:1.5em;}"
  ""
  "" /* Da utilizzare sulle pagine con una SOLA COLONNA. Sintassi (top,right,botton,left) */
  "#content_onecol{padding: 30px 10px 10px 10px;}"
  ""
  "#content.clean{background-color:transparent;border:none;}"
  " html > body #content{background-color:#FFFFCC;width:815px;}"                       /* Colore sfondo pagina */
  "#content  li{padding:0px;margin-left:5px;margin-bottom:0px;}"
  "#col2right ul > li{list-style:disc;}"
  ""
  "#footer{clear:both;height:53px;background-image: url(/image?png=footer);background-repeat:no-repeat;color:#3e1c0a;"
  " position:relative;width:817px;z-index:100;border-top:1px solid #404041;background-color:#2f292a;}"
  "#footer p{text-align:center;margin:0px;padding:10px;}"
  "#footer a{text-decoration:underline;}"
  "#p6{background-color:#2f292a;clear:both;}"
  "h1, h2  { color:#5C5D60; }"
  "h3, h4, h5, h6 { color:#F08121; }"
  "h1 {font-weight:bold;font-size:24px;}"
  "h3 { font-size:17px; }"
  "h4 { font-size:15px; }"
  "h5 { font-size:12px; }"
  "h6 { font-size:11px; }"
  "h4 { margin-bottom:12px; }"
  "h5 { margin-bottom:6px; }"
  "hr { clear:both; }"
  "li.head { color:#F08121; font-weight:bold; }"
  "sub{position:relative;top:-4px;display:inline;}"
  "#content p { color:#404041;}"
  ""
  "" /* Per immagine cliccabile pretty_version */
  "#col_rght p.dl{text-align:center;cursor:pointer;}"
  "p.dl {height:80px;border-top:1px solid #404041;border-bottom: 1px solid #404041;padding:10px 0px 10px 0px;}"
  "p.dl strong, p.dl strong a { font-size:14px; color:#404041; }"
  "p.dl span { float:left; }"
  "p.dl img{margin:0px;padding:0px;float:left;vertical-align:middle;}"
  ""
  "" /* Per immagine cliccabile dei loghi HOME */
  "p.dlh {height:25px;border-top:0px solid #404041;border-bottom: 0px solid #404041;padding:1px 0px 1px 0px;}"
  "p.dlh strong, p.dlh strong a { font-size:14px; color:#404041; }"
  "p.dlh span { float:left; }"
  "p.dlh img{margin:0px;padding:0px;float:left;vertical-align:middle;}"
  "p.intro{font-size:13px;line-height:1.8em;padding-bottom:15px;}"
  ".dlitem { height:30px; }"
  ".bline{ border-bottom:1px solid #404041; }"
  "code{display:block;border: 1px solid #404041;background-color:#eeeeee;padding:10px;margin-bottom:8px;}"
  "#content p.head { color:#F08121; font-weight:bold; }"
  "#content ul{margin:0px;padding:0px;list-style:none;padding-left:10px;}"
  "#content ul.normal{padding-left:30px;list-style:disc;}"
  "#content li{margin-bottom:5px;padding:2px 0px 2px 5px;}"
  "#content li.active{padding:2px 0px 2px 5px;margin-bottom:5px;background-color:#ddd;color:#404041;font-weight:bold;}"
  "#content li.active a{background-color:#ddd;color:#404041;}"
  "#content li a { color:#5C5D60; }"
  ""
  "#content h5	{ color:#5C5D60; }"
  "#content a  { text-decoration:underline; }"
  "body.about p  { line-height:1.6em; }"
  "body.about p  { margin-bottom:15px; }"
  "body.news #col2left h5 { color:#F08121;}"
  "#newsitem{border-bottom:1px dashed  #404041;margin-bottom:16px;}"
  "#newsitem b { display:block; }"
  "#newsitem i { display:block; }"
  "#newsitem p { clear:both; }2"
  "input {border-bottom:1px solid #ddd;border-right:1px solid #c3c3c3;border-top:1px solid #7c7c7c;"
  "border-left:1px solid #b2b2b2;font-size:11px;}"
  "#row{clear:both;position:relative;height:20px;}"
  "#label{float:left;width:150px;}"
  "#label.small { width: 80px; float:left;}"
  "#label.medium{ width:130px; float:left; }"
  "#item  { width: 170px; float:left;}"
  "#item input { font-size:10px; width:250px;}"
  "#item input.mini { width: 25px; }"
  "#item input.minimed { width: 40px; }"
  "#item input.small { width: 60px; }"
  "#item input.medium { width: 100px; }"
  "#item input.large { width:300px; font-size:12px; }"
  "#item textarea.large { width:300px;}"
  "#item input.button {width:auto;border-bottom:1px solid #7c7c7c;border-right:1px solid #b2b2b2;"
  "border-top:1px solid #ddd;border-left:1px solid #c3c3c3;font-size:11px;}"
  "#row.big #item { width: 370px; float:left; padding:3px;}"
  "select.large { width:300px; }"
  "#content #col_left{position:relative;width: 430px;float:left;margin-left:10px;margin-right:10px;z-index:10;"
  "padding:10px;}"
  "#content #col_rght{position:relative;width: 304px;float:right;z-index:10;padding:10px;}"
  "#content #col2left{width: 190px;float:left;margin-left:10px;z-index:10;padding:10px;}"
  "#content #col2left a {text-decoration:none;}"
  "#content #col2right{width: 510px;float:left;margin-left:10px;z-index:10;padding:10px;}"
  "#col2left li{list-style:none;padding:0px;margin:0px 0px 5px 0px;}"
  "#col2left h5{margin-bottom:3px;}"
};

#endif /*_CSS_H_*/
