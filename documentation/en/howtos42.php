<?php
# dvdisaster: English homepage translation
# Copyright (C) 2004-2010 Carsten Gnörlich
#
# UTF-8 trigger: äöüß 
#
# Include our PHP sub routines, then call begin_page()
# to start the HTML page, insert the header, 
# navigation and news if appropriate.

require("../include/dvdisaster.php");
require("../include/screenshot.php");

begin_page();

howto_headline("Recovering media images", "Walkthrough", "images/fix-icon.png");
?>

<!--- Insert actual page content below --->

Please make sure that dvdisaster has been configured as described in the
<a href="howtos41.php">basic settings</a> section.
Then perform the following steps:<p>

<hr>

<a name="a"></a>
<table>
<tr>
<td width="200px" align="center"><img src="../images/slot-in.png">
<br><img src="../images/down-arrow.png"></td>
<td>&nbsp;&nbsp;</td>
<td valign="top"><b>Insert the defective medium into a drive</b> 
which is directly connected to your computer. You can not use network drives,
software drives and drives inside virtual machines.</td>
</tr>
</table>

<table>
<tr>
<td width="200px" align="center"><img src="../images/winbrowser.png">
<br><img src="../images/down-arrow.png"></td>
<td>&nbsp;&nbsp;</td>
<td valign="top"><b>Close any windows</b> which may be opened by your operating system
for viewing or performing the medium contents.
Wait until the drive has recognized the medium and  the medium has spun down.</td>
</tr>
</table>

<table>
<tr>
<td width="200px" align="center"><a href="howtosa1.php">
<img src="../images/select-drive.png" border="0">
<br><img src="../images/down-arrow.png" border="0"></a></td>
<td>&nbsp;&nbsp;</td>
<td valign="top"><b>Select the drive</b> containing the defective medium
in dvdisasters drop down menu.</td>
</tr>
</table>

<table>
<tr>
<td width="200px"align="center">
<img src="../images/select-ecc.png" border="0" align="center"></a></td>
<td>&nbsp;&nbsp;</td>
<td valign="top">
If you are using <a href="howtos20.php">error correction files</a>
enter the file name in the shown field. 
Leave this entry blank when the medium has been
<a href="howtos30.php">augmented with error correction data</a>.<br>
</td>
</tr>
<td width="200px" align="center"><a href="howtosa1.php">
<img src="../images/down-arrow.png" border="0"></a></td>
</table>

<table>
<tr>
<td width="200px" align="center">
<a href="howtosa4.php">
<img src="images/read-icon.png" border="0">
<br><img src="../images/down-arrow.png" border="0"></a></td>
<td>&nbsp;&nbsp;</td>
<td valign="top"><b>Click the "Read" button</b> to start the reading process.</td>
</tr>
</table>

<?php begin_howto_shot("Reading the medium.","adaptive-progress.png", ""); ?>
<b>Watch the reading process progress.</b>
The adaptive reading strategy performs a systematic search for readable
sectors. You will observe temporary gaps which will be closed in later
stages. Usually this effect is less pronounced as shown in the screen shot. If all defective
sectors are located at the end of the medium the reading process may even stop before touching
the first defective sector.
<?php end_howto_shot(); ?>
<p>

<table>
<tr>
<td width="200px" align="center">
<img src="../images/down-arrow.png" border="0"></a></td>
</tr>
</table>

<?php begin_howto_shot("Reading process successful.","adaptive-success.png", ""); ?>
<b>The next actions depend on the outcome of the reading process.</b> 
The reading process terminates automatically when enough data for a successful recovery
has been gathered (compare the output marked in green). In that case continue the recovery
by clicking on the "Fix" button as described below.
<?php end_howto_shot(); ?>

<?php begin_howto_shot("Reading process failed.","adaptive-failure.png", ""); ?>
The reading process will also abort if it could not find enough readable sectors
(see the output marked in red). The image is <b>not</b> yet recoverable in this incomplete state.
Please try to gather additional data following the tips shown in
<a href="howtos43.php">advanced settings</a>.
<?php end_howto_shot(); ?>

<table>
<tr>
<td width="200px" align="center"><img src="../images/down-arrow.png" border="0"></td>
<td></td><td></td>
</tr>

<a name="b"></a>
<tr>
<td width="200px" align="center"><a href="howtosa4.php">
<img src="images/fix-icon.png" border="0">
</td>
<td>&nbsp;&nbsp;</td>
<td valign="top">Click the "Fix" button to begin the
<b>image recovery</b> (works <b>only</b> if the above reading process stated success!).</td>
</tr>

<tr>
<td width="200px" align="center"><img src="../images/down-arrow.png" border="0"></td>
<td></td><td></td>
</tr>
</table>

<?php begin_howto_shot("Watch the recovery.","fix-success.png", ""); ?>
<b>Watch the progress of the recovery.</b> The adaptive reading will stop as soon
as enough data has been collected for a successful recovery; therefore the error correction will
always be loaded to the max. This causes the display of the massive red area in the "Errors/Ecc block"
graph and is no cause for worry. Depending on the medium size and your system speed the recovery may take
several minutes to hours.
<?php end_howto_shot(); ?>

<table>
<tr>
<td width="200px"align="center">
<img src="../images/down-arrow.png" border="0">
</td>
</tr>
</table>

<table>
<tr>
<td width="200px"align="center">
<img src="../images/good-image.png" border="0" align="center"></a></td>
<td>&nbsp;&nbsp;</td>
<td valign="top">After the recovery finishes all data in the ISO image will be complete again.
</td>
</tr>
</table>

<table>
<tr>
<td width="200px"align="center">
<img src="../images/down-arrow.png" border="0">
</td>
</tr>
</table>

<table>
<tr>
<td width="200px" align="center">
<a href="howtos33.php?way=2#c">
<img src="thumbnails/write-iso1.png" border="0"></a></td>
<td>&nbsp;&nbsp;</td>
<td valign="top"><b>Write the recovered ISO image</b> 
to a new medium. Perform the same steps as described in the section
about <a href="howtos33.php?way=2#c">writing media</a> which have been
<a href="howtos33.php">augmented with error correction data</a>.
</td>
</tr>
</table>

<table>
<tr>
<td width="200px"align="center">
<img src="../images/down-arrow.png" border="0">
</td>
</tr>
</table>

<table>
<tr>
<td width="200px"align="center">
<img src="../images/old-cd.png" border="0" align="center">
<img src="../images/old-image.png" border="0" align="center">
<img src="../images/good-cd.png" border="0" align="center"></a></td>
<td>&nbsp;&nbsp;</td>
<td valign="top">Now you have created a new medium containing the fully recovered data.
Make sure to <a href="howtos10.php">check it for read errors</a>. 
Then you can discard the defective medium and delete the ISO image. However if you have created
an error correction file for the old medium then you can keep it to protect the newly
created medium.
</td>
</tr>
</table>

<!--- do not change below --->

<?php
# end_page() adds the footer line and closes the HTML properly.

end_page();
?>
