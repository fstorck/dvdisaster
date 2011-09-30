<?php
# dvdisaster: German homepage translation
# Copyright (C) 2004-2010 Carsten Gnörlich
#
# UTF-8 trigger: äöüß 
#
# Include our PHP sub routines, then call begin_page()
# to start the HTML page, insert the header, 
# navigation and news if appropriate.

require("../include/dvdisaster.php");
require("../include/footnote.php");
begin_page();
$show_all=$_GET["showall"];
?>

<!--- Insert actual page content below --->

<h3>dvdisaster herunterladen</h3>

dvdisaster unterstützt die Betriebssysteme Darwin/Mac OS X, FreeBSD, GNU/Linux, NetBSD und Windows in den <a href="download10.php">aktuellen Versionen</a>.
Es wird Ihnen 
als <a href="http://www.germany.fsfeurope.org/documents/freesoftware.de.html">freie Software</a> 
unter der <a href="http://dvdisaster.cvs.sourceforge.net/dvdisaster/dvdisaster/COPYING?view=markup">GNU General Public License v2</a><a href="#gpl3"><sup>*)</sup></a> zur 
Verfügung gestellt.<p>

Laden Sie sich bitte entweder den Quellkode oder eine Binärversion 
aus der folgenden Liste herunter. Die Pakete können mit
einer <a href="download20.php">digitalen Unterschrift</a> auf 
ihren Ursprungszustand überprüft werden.<p>

<ul>
<li>Die Quellkode-Version enthält eine Datei <tt>INSTALL</tt> mit weiteren
Informationen zum Übersetzen des Programmes.</li>
<li>Für Mac OS X laden Sie bitte das ZIP-Archiv herunter und packen es an einer
beliebigen Stelle aus. Bitte beachten Sie die 
<a href="download30.php#mac">speziellen Hinweise für Mac OS X</a>.</li>
<li>Um die Binärversion <a href="download30.php#win">für Windows zu installieren</a>, 
rufen Sie das Programm nach dem Herunterladen auf und folgen dem Dialog.</li>
</ul> 

<!---
<b>Alpha-/Entwickler-Versionen</b> - neu und experimentell für erfahrene Benutzer!<p> 

Sie sind eingeladen, die nächste dvdisaster-Version auszuprobieren, 
aber beachten Sie bitte, daß diese Version noch Fehler und 
Kompatibilitätsprobleme enthalten kann. Die neueste experimentelle Version 
ist <a href="download40.php"><?php echo $cooked_version ?></a>. 
--->

<pre> </pre>

<b>Stabile Version</b> - zum Einstieg empfohlen<p> 

<table class="download" cellpadding="0" cellspacing="5">
<tr><td><b>dvdisaster-0.72</b></td><td align="right">31-Okt-2010</td></tr>
<tr bgcolor="#000000"><td colspan="2"><img width=1 height=1 alt=""></td></tr>
<tr><td colspan="2">
  <table>
    <tr><td align="right">&nbsp;&nbsp;Quellkode für alle Betriebssysteme:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.2.tar.bz2">dvdisaster-0.72.2.tar.bz2</a></td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.2.tar.bz2.gpg">dvdisaster-0.72.2.tar.bz2.gpg</a></td></tr>
    <tr><td align="right">Binärversion für Mac OS X 10.5 / x86:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.2.app.zip">dvdisaster-0.72.2.app.zip</a>&nbsp;--&nbsp;bitte erst den <a href="download30.php#mac">Hinweis</a> lesen</td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.2.app.zip.gpg">dvdisaster-0.72.2.app.zip.gpg</a></td></tr>
    <tr><td align="right">Binärversion für Windows:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.2-setup.exe">dvdisaster-0.72.2-setup.exe</a></td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.2-setup.exe.gpg">dvdisaster-0.72.2-setup.exe.gpg</a></td></tr>
    <tr><td colspan="2"> </td></tr>
<?php
  if($show_all == 0) {
?>
    <tr><td colspan="2"><a href="download.php?showall=1">Ältere Veröffentlichungen des 0.72er-Versionszweiges anzeigen</a></td></tr>
<?php
  }
  else {
?> 
   <tr><td colspan="2"><a href="download.php?showall=0">Ältere Veröffentlichungen des 0.72er-Versionszweiges verbergen</a></td></tr>


    <tr><td align="right">&nbsp;&nbsp;Quellkode für alle Betriebssysteme:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.1.tar.bz2">dvdisaster-0.72.1.tar.bz2</a></td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.1.tar.bz2.gpg">dvdisaster-0.72.1.tar.bz2.gpg</a></td></tr>
    <tr><td align="right">Binärversion für Mac OS X 10.5 / x86:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.1.app.zip">dvdisaster-0.72.1.app.zip</a>&nbsp;--&nbsp;bitte erst den <a href="download30.php#mac">Hinweis</a> lesen</td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.1.app.zip.gpg">dvdisaster-0.72.1.app.zip.gpg</a></td></tr>
    <tr><td align="right">Binärversion für Windows:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.1-setup.exe">dvdisaster-0.72.1-setup.exe</a></td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://dvdisaster.net/downloads/dvdisaster-0.72.1-setup.exe.gpg">dvdisaster-0.72.1-setup.exe.gpg</a></td></tr>
   <tr><td colspan="2"> </td></tr>
    <tr><td align="right">&nbsp;&nbsp;Quellkode für alle Betriebssysteme:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.72.tar.bz2?download">dvdisaster-0.72.tar.bz2</a></td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.72.tar.bz2.gpg?download">dvdisaster-0.72.tar.bz2.gpg</a></td></tr>
    <tr><td align="right">Binärversion für Mac OS X 10.5 / x86:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.72.app.zip?download">dvdisaster-0.72.app.zip</a>&nbsp;--&nbsp;bitte erst den <a href="download30.php#mac">Hinweis</a> lesen</td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.72.app.zip.gpg?download">dvdisaster-0.72.app.zip.gpg</a></td></tr>
    <tr><td align="right">Binärversion für Windows:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.72-setup.exe?download">dvdisaster-0.72-setup.exe</a></td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.72-setup.exe.gpg?download">dvdisaster-0.72-setup.exe.gpg</a></td></tr>
<?php
  }
?>
  </table>
</td></tr>
<tr bgcolor="#000000"><td colspan="2"><img width=1 height=1 alt=""></td></tr>
<tr><td colspan="2">
Grundlegende Neuerungen in dieser Version:<p>
<ul>
<li>Unterstützung für <a href="qa10.php#media">Blu-Ray-Datenträger</a></li>
<li>"Raw"-Lesen und C2-Überprüfungen für CD-Datenträger</li>
<li>Einstellbare Anzahl der Leseversuche</li>
<li>Erstes "natives" Mac OS X-Paket </li>
<li>NetBSD-Port von Sergey Svishchev</li>
<li>Verbesserte Typerkennung für eingelegte Datenträger</li>
<li>Informationsfenster für eingelegten Datenträger</li>
<li>Überarbeiteter und erweiterter Dialog für Programmeinstellungen</li>
<li>Neu gestaltete und erweiterte Dokumentation</li>
<li>Russische Übersetzungen von Igor Gorbounov</li>
<li>... und unzählige weitere kleine Sachen.</li>
</ul>

<b>Patches</b> (kleine Änderungen nach Version 0.72; die obigen Dateien wurden erneuert):<p>

<b>0.72 pl2</b> Diese Version führt einen Workaround ein
um zu verhindern daß parallele SCSI-Adapter unter Linux
nicht mehr reagieren. Die Kompatibilität mit dem
neuen Versionszweig 0.79.x wurde verbessert. (31-Okt-2010)<P>

<b>0.72 pl1</b> Pablo Almeida hat die Bildschirmtexte ins Portugiesische übersetzt.
Es wurde ein Workaround eingebaut um das Einfrieren von Windows XP bei bestimmten Kombinationen
von CD-RW-Rohlingen und Laufwerken zu verhindern. (08-Aug-2009)<p>

<b>0.72</b> Dies ist die erste stabile Version des 0.72er-Zweiges.
Igor Gorbounov hat die russische online-Dokumentation vervollständigt
und es wurden noch einige kleine Fehler aus dem ersten 
Veröffentlichungskandidaten behoben.<p>
Unter neueren Windows-Versionen wird bei bestimmten Spracheinstellungen
nicht die gewünschte Bildschirmsprache dargestellt. Die Behebung
dieses Problems ist ziemlich komplex und wird erst mit Version 0.73
erfolgen. (04-Jul-2009)<p>
 
<b>0.72-rc1</b> Erster Kandidat für die stabile Version. (11-Apr-2009)
</td></tr></table><p>

<b>Vorangegangene Version</b> - eine Aktualisierung auf Version 0.72 wird empfohlen.<p> 

<table class="download" cellpadding="0" cellspacing="5">
<tr><td><b>dvdisaster-0.70</b></td><td align="right">04-Mär-2008</td></tr>
<tr bgcolor="#000000"><td colspan="2"><img width=1 height=1 alt=""></td></tr>
<tr><td colspan="2">
  <table>
    <tr><td align="right">&nbsp;&nbsp;Quellkode für alle Betriebssysteme:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.70.6.tar.bz2?download">dvdisaster-0.70.6.tar.bz2</a></td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.70.6.tar.bz2.gpg?download">dvdisaster-0.70.6.tar.bz2.gpg</a></td></tr>
    <tr><td align="right">Binärversion für Windows:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.70.6-setup.exe?download">dvdisaster-0.70.6-setup.exe</a></td></tr>
    <tr><td align="right">Digitale Unterschrift:&nbsp;</td>
        <td><a href="http://prdownloads.sourceforge.net/dvdisaster/dvdisaster-0.70.6-setup.exe.gpg?download">dvdisaster-0.70.6-setup.exe.gpg</a></td></tr>
  </table>
</td></tr>
<tr bgcolor="#000000"><td colspan="2"><img width=1 height=1 alt=""></td></tr>
<tr><td colspan="2">
Das RS02-Fehlerkorrektur-Verfahren
wird in der graphischen Benutzeroberfläche vollständig unterstützt.
Damit erzeugte Abbilder können nun auch
mit dem angepaßten Leseverfahren verarbeitet werden.<p>

Julian Einwag hat damit begonnen, dvdisaster 
für Mac OS X / Darwin anzupassen.<p>

Daniel Nylander hat die Bildschirmtexte ins Schwedische übersetzt.<p>

<b>Patches</b> (kleine Änderungen nach Version 0.70; die obigen Dateien wurden erneuert):<p>

<b>pl6</b> Die Unterstützung von Umlauten in Dateinamen wurde
wieder rückgängig gemacht,
da sie zur fehlerhaften Bearbeitung von Dateien &gt; 2GB unter Windows führte. 
Eine korrekte Behandlung von Dateinamen mit Sonderzeichen
 wird jetzt erst in der
experimentellen Version 0.71.25 erprobt. <i>(04-Mär-2008)</i><p>

<b>pl5</b> Behebt ein Problem mit neueren Linux-Kernen, das unter bestimmten Bedingungen zum
Einfrieren des Systems führen kann. Die Behandlung von Umlauten in Dateinamen wurde
verbessert.
Zurückportierung einiger weiterer
Verbesserungen aus 0.71.24. <i>(24-Feb-2008)</i>.<p>

<b>pl4</b> ist besser kompatibel zu zweischichtigen DVDs 
(DVD-R DL und DVD+R DL).<br> 
Einige kleinere Fehler wurden behoben. <i>(20-Jan-2007)</i>.<p>

<b>pl3</b> behebt einen Fehler bei der Erkennung von nicht unterstützten CDs,
der unter Umständen einen blauen Bildschirm unter Windows erzeugte.
Abbruchmöglichkeit während der RS02-Erkennung für DVD RW hinzugefügt.
<i>(10-Dez-2006)</i>.<p>

<b>pl2</b> behebt eine fehlerhafte Speicherfreigabe beim Schließen des Programms.
Das Auspacken der Bildschirmfotos für die Dokumentation auf PPC-Plattformen wurde
korrigiert. Nur die Quellkode-Archive wurden erneuert. 
<i>(03-Okt-2006)</i>.<p>

<b>pl1</b> behebt einen Fehler im angepaßten Lesen für RS02, durch den unter bestimmten
Bedingungen nicht genügend Daten für eine erfolgreiche Wiederherstellung gelesen wurden.
Ein paar kleine Verbesserungen an der Dokumentation und der Benutzbarkeit sind auch dabei.
<i>(30-Jul-2006)</i>
</td></tr></table><p>

Der Quellkode von dvdisaster kann auch direkt im 
<a href="http://sourceforge.net/cvs/?group_id=157550">CVS-Archiv</a>
betrachtet werden. Einige interessante Dateien darin sind: 
<ul>
<li><a href="http://dvdisaster.cvs.sourceforge.net/dvdisaster/dvdisaster/CHANGELOG?view=markup">CHANGELOG</a>- was hat sich in den letzten Versionen geändert (in Englisch);</li>
<li><a href="http://dvdisaster.cvs.sourceforge.net/dvdisaster/dvdisaster/CREDITS.de?view=markup">CREDITS.de</a>- wer bei dem Projekt bisher mitgemacht hat;</li>
<li><a href="http://dvdisaster.cvs.sourceforge.net/dvdisaster/dvdisaster/INSTALL?view=markup">INSTALL</a> - Installationshinweise (in Englisch);</li>
<li><a href="http://dvdisaster.cvs.sourceforge.net/dvdisaster/dvdisaster/README?view=markup">README</a> - eine Übersicht zum Quellkodearchiv (in Englisch).</li>
</ul>

<pre> </pre>

<?php
footnote("*","gpl3","dvdisaster wird zu einem späteren Zeitpunkt zur GNU General Public License Version 3 wechseln.");
?>

<!--- do not change below --->

<?php
# end_page() adds the footer line and closes the HTML properly.

end_page();
?>
