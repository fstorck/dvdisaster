<?php
# dvdisaster: English homepage translation
# Copyright (C) 2004-2010 Carsten Gnörlich
#
# UTF-8 trigger: äöüß 
#
# Include our PHP sub routines, then call begin_page()
# to start the HTML page, insert the header, 
# navigation and news if appropriate.
?>

<a name="filechooser"></a>
<b>Замечания к использованию диалога выбора файлов в среде Windows и Mac OS X</b><p>

dvdisaster использует набор средств для создания пользовательского интерфейса <a href="http://www.gtk.org">GTK+</a>, который работает на множестве различных операционных систем. Поскольку
диалог выбора файлов в GTK+ работает не так, как его аналоги в Windows и Mac OS X (которые мы не можем использовать), дальше на этой странице дается краткое знакомство с ним.
Имейте в виду, что для создания версии под Windows или Mac OS X без GTK+ потребовались бы колоссальные усилия - мы, скорее всего, попросту бы сконцентрировались на 
версии для GNU/Linux ;-)<p>

<?php begin_screen_shot("Выбор файлов в версии для Windows","filebrowser.png"); ?>

<b>Выбор существующих файлов.</b>
Области, отмеченные <font color="#008000">зеленым,</font> используются для выбора существующих файлов.
Перемещение по каталогам и их выбор делается в левой части диалога; содержащиеся здесь
файлы показываются и выбираются в правой половине.</b><p>

<b>Выбор имени и местоположения новых файлов.</b> 
Сначала выберите каталог для нового файла, используя 
<font color="#008000">зеленую</font> область в левой половине диалогового окна.
Затем введите имя нового файла в  
текстовом поле, отмеченном <font color="0000A0">голубым</font>. 
Чтобы еще раз убедиться, что вы создаете файл в правильном месте, посмотрите
на надпись после "Выбор:" над <font color="0000A0">голубым</font> полем; в ней содержится
буква диска и полный каталог, в котором будет создаваться новый файл.<p>

<b>Переключение между разделами ("буквами дисков").</b>

Имеющиеся разделы перечисляются в  
области, помеченной <font color="#e0b000">желтым</font>. Если текущий диск
содержит множество подкаталогов, то вам придется, наверное, прокрутить вниз, чтобы увидеть буквы дисков.<p>

<b>Возврат в вышестоящий каталог.</b>
Нажмите мышкой на две точки (..), отмеченные <font color="#a00000">красным,</font>
для возврата на один уровень каталогов. Все каталоги, ведущие к текущему,
содержатся в выпадающем меню, расположенном в центре верхней части диалогового окна
(тоже отмечено <font color="#a00000">красным</font>). 
Имейте в виду, что это выпадающее меню <i>не</i> используется для переключения букв дисков;
используйте для этого <font color="#e0b000">желтую</font> часть списка выбора.

<?php end_screen_shot(); ?>
