

//// xcopy /옵션		.복사할파일이 담겨있는 경로		.복사 할 경로 

pushd %~dp0

xcopy /y/s .\Engine\Bin\*.lib				.\Client\Bin\
xcopy /y/s .\Engine\Bin\*.dll				.\Client\Bin\