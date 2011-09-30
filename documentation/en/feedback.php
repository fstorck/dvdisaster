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
?>

<!--- Insert actual page content below --->

<h3>Bug reporting</h3>
Like all complex software, dvdisaster may contain bugs (programming errors) and
incompatibilities with certain (drive) hardware and software setups.
You are invited to tell us about any difficulties you encounter with the program
or the documentation so that we can improve things in future releases.<p>

To make sure that we are getting the right information, we have provided the
following checklist for bug reporting:


<h4>Please check first that you are really experiencing a bug:</h4>

<ul>
<li>Make sure that you are using the latest genuine version from our
<a href="http://dvdisaster.net/en/download.php">download site 
at SourceForge</a>.
dvdisaster versions provided by third parties
may contain functions and bugs which are not present in the original version
(and we can't fix their problems).</li>
<li>Double check that the issue you have encountered is not already covered in the
<a href="qa20.php">Questions and Answers</a> section.</li>
<li>Please note that dvdisaster will only work with the (re-)writeable varieties
of media, so seeing it <b>reject</b> <b>DVD-ROM</b> and <b>BD-ROM</b> 
is <b>not an error</b>. 
Also, CD-Audio, VCD, SVCD and multisession CD are not supported as well as all
HD-DVD formats
(<a href="qa10.php#media">complete list of supported media formats</a>).</li>
<li>dvdisaster works only with real drives. Not supported are
network drives, software drives (e.g. alcohol) and drives in virtual 
machines.</li>
</ul>

<h4>How to report issues with the program:</h4>

Please report your findings by sending an email to 
<img src="../images/email.png" align="top">. Your report should contain:<p>

<ul>
<li>Information about the operating system and dvdisaster version you are using;</li>
<li>the drive and media type(s) which exhibited the problem; </li>
<li>a textual description of the issue you encountered;</li>
<li>a screen shot of the error message and/or output which might provide
further information about the problem;</li>
<li>differences between working and non-working configurations if the issue is
experienced only on certain drives/computers;</li>
<li>a log file if you suspect that the issue is related to a drive or medium
incompatibility.</li>
</ul>

<?php begin_screen_shot("Creating a log file.", "activate-logfile.png"); ?>

<b>How to create a log file:</b> If you suspect incompatibilities with your drive and/or media
as the cause of your issue, please activate the log file feature in the preferences 
dialog as shown in the screen shot. Then perform a scanning or reading action 
and attach the log file to your bug report.
<?php end_screen_shot(); ?>

Thanks for your feedback!

<!--- do not change below --->

<?php
# end_page() adds the footer line and closes the HTML properly.

end_page();
?>
