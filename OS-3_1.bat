@echo off
chcp 1251
rem cd %homepath%\Desktop
cls

set argc=0

set key_ru=0
set key_b=0
set key_e=0
set key_h=0

set FAILURE_MSG=SUCCESSFUL

set str_i=_EmPtY_
set output_file=_EmPtY_

for %%c IN (%*) DO (
   set /a argc+=1

   if %%c == /ru set   	key_ru=1
   if %%c == /b  set    key_b=1
   if %%c == /begin  set    key_b=1
   if %%c == /e  set    key_e=1
   if %%c == /end  set    key_e=1
   if %%c == /h  set    key_h=1
   if %%c == /help  set    key_h=1
   if %%c == /?  set    key_h=1
)

if %key_b% ==1 if %key_e% ==0 (
 if %key_ru% == 1 (
	   
	set /p output_file="Укажите файл в который необходимо добавить строку: "
  )else (
	set /p str_i="Enter the line to add: "
	set /p output_file="Specify the file to which you want to add a line: "
  )
)
 if %key_b% ==0 if %key_e% ==1 (
 if %key_ru% == 1 (
	set /p str_i="Введите добавляемую строку: "
	set /p output_file="Укажите файл в который необходимо добавить строку: "
  )else (
	set /p str_i="Enter the line to add: "
	set /p output_file="Specify the file to which you want to add a line: "
  )
)

if %argc%  == 0  goto help
if %argc%  == 1  if %key_ru% == 1 goto help
if %key_h% == 1  goto help


if %key_e%==1 if %key_b%==1 (
   if %key_ru% == 1 (
	set FAILURE_MSG=Использование двух ключей /b и /e недопустимо!
   )
   if %key_ru% == 0 (
	set FAILURE_MSG=cannot use the /b and /e keys together!
   )
   goto failure
)


:rec
if %key_e%==1		echo %str_i%>>%output_file%
if %key_b%==1 		echo %str_i%>%output_file%

goto end


:end

@echo on
exit /b


:failure
echo [%0]: %FAILURE_MSG%
goto end


:help
echo %cd%\%0
if %key_ru% == 1 (
	echo Использование: /nOS-1_1 [/ru] [[/h]или [/?]] [[/b]или[/e]] [/i входная_строка] [/o файл_вывода]	
	echo Параметры: 
	echo /ru			вывод сообщений на русском языке
	echo /h or /?		ключи помощи
	echo /e			вставка сообщения в начало
	echo /b			вставка сообщения в конец 
 	echo /i входная_строка	ключ для входной строки записываемой в файл
	echo /o файл_вывода		ключ для вывода информации в файл
)else (
	echo Usage: /nOS-1_1 [/ru] [[/h]or [/?]] [[/b]or [/e]] [/i input_file] [/o output_file]
	echo Parametrs:
	echo /ru			output messages in Russian
	echo /h or /?		help keys
	echo /e			inserting a message at the beginning
	echo /b			inserting a message at the end
 	echo /i input_file		key for the input string to be written to the file
	echo /o output_file		key to output information to a file
)
goto end 