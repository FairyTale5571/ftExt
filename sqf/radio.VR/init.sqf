addMissionEventHandler ["HandleChatMessage", {
	params ["_channel", "_owner", "_from", "_text", "_person", "_name", "_strID", "_forcedDisplay", "_isPlayerMessage", "_sentenceType", "_chatMessageType"];
	diag_log format ["SYSCHAT >>: %1",_text];
}];

addMissionEventHandler ["ExtensionCallback",{
	params ["_name","_function","_data"];
	systemChat format ["N: %1 | F: %2 | D: %3",_name,_function,_data];
	diag_log format ["N: %1 | F: %2 | D: %3",_name,_function,_data];
}];

radioPlay = {
	_cur = cursorObject;
	if (isNull _cur) exitWith {hint "Наведитесь сначала на объект"};
	_display = uiNamespace getVariable ["RRPDialogRadio",displayNull];
	_text = _display displayCtrl 1100;
	_list = _display displayCtrl 1500;
	
	_link = lbData[1500,lbCurSel 1500]; 
	if (_link isEqualTo "") exitWith {hint "Ничего не выбрано"};
	_station = lbText[1500,lbCurSel 1500];
	_pos = getPosWorld _cur;
	"ftExt" callExtension ["create",[str _link,"100", str(_pos # 0), str(_pos # 1), str(_pos # 2)]];
	// _rad = "ftExt" callExtension ["urlInfo",[]] select 0;
	// if (_rad isEqualTo "Track: ") then {_rad = "Не удалось определить, попробуйте нажать 'Обновить'"};
	// _text ctrlSetStructuredText (parseText (format ["<t size='1.2'>Активная станция: %1</t><br/><t size='1.2'>Сейчас играет: %2</t>",_station,_rad]));
};
onLbSel = {
	_control = _this select 0;
	_index = _this select 1;
	_link = _control lbData _index;
};

radioStop = {
	"ftExt" callExtension ["destroy",[]];
	_display = uiNamespace getVariable ["RRPDialogRadio",displayNull];
	_text = _display displayCtrl 1100;
	_text ctrlSetStructuredText (parseText (format ["<t size='1.2'>Радио выключено</t>"]));
};
refreshRadio = {
	_ret = "ftExt" callExtension ["urlInfo",[]];
	_display = uiNamespace getVariable ["RRPDialogRadio",displayNull];
	_text = _display displayCtrl 1100;
	_text ctrlSetStructuredText (parseText (format ["<t size='1.2'>Сейчас играет: %1</t>",_ret]));
};
openRadio = {
	createDialog "RRPDialogRadio";
	_display = uiNamespace getVariable ["RRPDialogRadio",displayNull];
	_text = _display displayCtrl 1100;
	_text ctrlSetStructuredText (parseText (format ["<t size='1.2'>Радио выключено</t>"]));
	_list = _display displayCtrl 1500;
	{
		_name = _x select 0;
		_link = _x select 1;
		_index = _list lbAdd _name;
		_list lbSetData [_index,_link]; 
	}forEach (getArray(getMissionConfig "radios"));
	_list ctrlAddEventHandler ["LBSelChanged","_this call onLbSel"];
};
