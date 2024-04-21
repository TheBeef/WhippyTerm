<div id="Header">
    <div id="Logo">
        Whippy Term
    </div>
    <div id="HeaderInfo">
        <a href="/download.php">Early release available!</a>
    </div>
    <div id="Menu">
        <a href="/"                 <?php if($PageSelected==0) echo "class='Here'" ?>>Home</a>
        <a href="/download.php"     <?php if($PageSelected==1) echo "class='Here'" ?>>Download</a>
        <a href="/plugins.php"      <?php if($PageSelected==2) echo "class='Here'" ?>>Plugins</a>
        <a href="/About.php"        <?php if($PageSelected==3) echo "class='Here'" ?>>About</a>
        <a href="/License.php"      <?php if($PageSelected==4) echo "class='Here'" ?>>License</a>
        <a href="/FAQ.php"          <?php if($PageSelected==5) echo "class='Here'" ?>>FAQ</a>
        <a href="/Links.php"        <?php if($PageSelected==6) echo "class='Here'" ?>>Links</a>
        <a href="/Contact.php"      <?php if($PageSelected==7) echo "class='Here'" ?>>Contact</a>
    </div>
</div>
<div id="Title">
<?=$PageTitle?>
</div>
<div id="Content">
