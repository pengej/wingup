/*
 Copyright 2007 Don HO <don.h@free.fr>

 This file is part of GUP.

 GUP is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 GUP is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GUP.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "xmlTools.h"

using namespace std;
using namespace tinyxml2;

GupParameters::GupParameters(const char * xmlFileName)
{
	_xmlDoc.LoadFile(xmlFileName);

	XMLNode *root = _xmlDoc.FirstChildElement("GUPInput");
	if (!root)
		throw exception("It's not a valid GUP input xml.");

	XMLNode *displayNameNode = root->FirstChildElement("DisplayName");
	if (displayNameNode)
	{
		XMLNode *n = displayNameNode->FirstChild();
		if (n)
		{
			const char *val = n->Value();
			if (val)
			{
				_displayName = val;
			}
		}
	}

	XMLNode *versionNode = root->FirstChildElement("Version");
	if (versionNode)
	{
		XMLNode *n = versionNode->FirstChild();
		if (n)
		{
			const char *val = n->Value();
			if (val)
			{
				_currentVersion = val;
			}
		}
	}

	XMLNode *paramNode = root->FirstChildElement("Param");
	if (paramNode)
	{
		XMLNode *n = paramNode->FirstChild();
		if (n)
		{
			const char *val = n->Value();
			if (val)
			{
				_param = val;
			}
		}
	}
	
	XMLNode *infoURLNode = root->FirstChildElement("InfoUrl");
	if (!infoURLNode)
		throw exception("InfoUrl node is missed.");

	XMLNode *iu = infoURLNode->FirstChild();
	if (!iu)
		throw exception("InfoUrl is missed.");
		
	const char *iuVal = iu->Value();
	if (!iuVal || !(*iuVal))
		throw exception("InfoUrl is missed.");
	
	_infoUrl = iuVal;

	XMLNode *classeNameNode = root->FirstChildElement("ClassName2Close");
	if (classeNameNode)
	{
		XMLNode *n = classeNameNode->FirstChild();
		if (n)
		{
			const char *val = n->Value();
			if (val)
			{
				_className2Close = val;
			}
		}
	}

	XMLNode *progNameNode = root->FirstChildElement("MessageBoxTitle");
	if (progNameNode)
	{
		XMLNode *n = progNameNode->FirstChild();
        const char *valStr = NULL;

		if (n)
		{
			valStr = n->Value();
			if (valStr)
			{
				_messageBoxTitle = valStr;
			}
		}

		valStr = (progNameNode->ToElement())->Attribute("isModal");
		if (valStr)
		{
			if (stricmp(valStr, "yes") == 0)
				_isMessageBoxModal = true;
			else if (stricmp(valStr, "no") == 0)
				_isMessageBoxModal = false;
			else
				throw exception("isModal value is incorrect (only \"yes\" or \"no\" is allowed).");
		}

        int val = 0;
		XMLError errReturn = (progNameNode->ToElement())->QueryIntAttribute("extraCmd", &val);
		if (errReturn == XML_SUCCESS)
		{
			_3rdButton_wm_cmd = val;
		}
		
		errReturn = (progNameNode->ToElement())->QueryIntAttribute("ecWparam", &val);
		if (errReturn == XML_SUCCESS)
		{
			_3rdButton_wParam = val;
		}
		
		errReturn = (progNameNode->ToElement())->QueryIntAttribute("ecLparam", &val);
		if (errReturn == XML_SUCCESS)
		{
			_3rdButton_lParam = val;
		}

		const char * extraCmdLabel = (progNameNode->ToElement())->Attribute("extraCmdButtonLabel");
		if (extraCmdLabel != NULL)
		{
			_3rdButton_label = extraCmdLabel;
		}
	}

	XMLNode *silentModeNode = root->FirstChildElement("SilentMode");
	if (silentModeNode)
	{
		XMLNode *smn = silentModeNode->FirstChild();
		if (smn)
		{
			const char *smnVal = smn->Value();
			if (smnVal && *smnVal)
			{
				if (stricmp(smnVal, "yes") == 0)
					_isSilentMode = true;
				else if (stricmp(smnVal, "no") == 0)
					_isSilentMode = false;
				else
					throw exception("SilentMode value is incorrect (only \"yes\" or \"no\" is allowed).");
			}
		}
	}

	
	//
	// Get optional parameters
	//
	XMLNode *userAgentNode = root->FirstChildElement("SoftwareName");
	if (userAgentNode)
	{
		XMLNode *un = userAgentNode->FirstChild();
		if (un)
		{
			const char *uaVal = un->Value();
			if (uaVal)
				_softwareName = uaVal;
		}
	}
}

GupDownloadInfo::GupDownloadInfo(const char * xmlString) : _updateVersion(""), _updateLocation(""), _updateRunParams("")
{
	_xmlDoc.Parse(xmlString);

	XMLNode *root = _xmlDoc.FirstChildElement("GUP");
	if (!root)
		throw exception("It's not a valid GUP xml.");

	XMLNode *needUpdateNode = root->FirstChildElement("NeedToBeUpdated");
	if (!needUpdateNode)
		throw exception("NeedToBeUpdated node is missed.");

	XMLNode *nun = needUpdateNode->FirstChild();
	if (!nun)
		throw exception("NeedToBeUpdated is missed.");
		
	const char *nunVal = nun->Value();
	if (!nunVal || !(*nunVal))
		throw exception("NeedToBeUpdated is missed.");
	
	if (stricmp(nunVal, "yes") == 0)
		_need2BeUpdated = true;
	else if (stricmp(nunVal, "no") == 0)
		_need2BeUpdated = false;
	else
		throw exception("NeedToBeUpdated value is incorrect (only \"yes\" or \"no\" is allowed).");

	if (_need2BeUpdated)
	{
		//
		// Get mandatory parameters
		//
		XMLNode *versionNode = root->FirstChildElement("Version");
		if (versionNode)
		{
			XMLNode *n = versionNode->FirstChild();
			if (n)
			{
				const char *val = n->Value();
				if (val)
				{
					_updateVersion = val;
				}
			}
		}
		
		XMLNode *locationNode = root->FirstChildElement("Location");
		if (!locationNode)
			throw exception("Location node is missed.");

		XMLNode *ln = locationNode->FirstChild();
		if (!ln)
			throw exception("Location is missed.");
			
		const char *locVal = ln->Value();
		if (!locVal || !(*locVal))
			throw exception("Location is missed.");
		
		_updateLocation = locVal;

		XMLNode *runParamsNode = root->FirstChildElement("RunParams");
		if (runParamsNode) {
			XMLNode *ln = runParamsNode->FirstChild();
			if (ln) {
				const char *runParamsVal = ln->Value();
				if (runParamsVal && (*runParamsVal)) {
					_updateRunParams = runParamsVal;
				}
			}
		}		
	}
}

GupExtraOptions::GupExtraOptions(const char * xmlFileName) : _proxyServer(""), _port(-1)//, _hasProxySettings(false)
{
	_xmlDoc.LoadFile(xmlFileName);

	XMLNode *root = _xmlDoc.FirstChildElement("GUPOptions");
	if (!root)
		return;
		
	XMLNode *proxyNode = root->FirstChildElement("Proxy");
	if (proxyNode)
	{
		XMLNode *serverNode = proxyNode->FirstChildElement("server");
		if (serverNode)
		{
			XMLNode *server = serverNode->FirstChild();
			if (server)
			{
				const char *val = server->Value();
				if (val)
					_proxyServer = val;
			}
		}

		XMLNode *portNode = proxyNode->FirstChildElement("port");
		if (portNode)
		{
			XMLNode *port = portNode->FirstChild();
			if (port)
			{
				const char *val = port->Value();
				if (val)
					_port = atoi(val);
			}
		}
	}
}

void GupExtraOptions::writeProxyInfo(const char *fn, const char *proxySrv, long port)
{
	XMLDocument newProxySettings;
	XMLNode *root = newProxySettings.InsertEndChild(newProxySettings.NewElement("GUPOptions"));
	XMLNode *proxy = root->InsertEndChild(newProxySettings.NewElement("Proxy"));
	XMLNode *server = proxy->InsertEndChild(newProxySettings.NewElement("server"));
	server->InsertEndChild(newProxySettings.NewText(proxySrv));
	XMLNode *portNode = proxy->InsertEndChild(newProxySettings.NewElement("port"));
	char portStr[10];
	sprintf(portStr, "%d", port);
	portNode->InsertEndChild(newProxySettings.NewText(portStr));

	XMLError errSaveFile = newProxySettings.SaveFile(fn);
	if (errSaveFile != XML_SUCCESS)
	{
		throw exception("Proxy settings can't be saved to GUPOptions.xml .");
		return;
	}
}

std::string GupNativeLang::getMessageString(std::string msgID)
{
	if (!_nativeLangRoot)
		return "";

	XMLNode *popupMessagesNode = _nativeLangRoot->FirstChildElement("PopupMessages");
	if (!popupMessagesNode)
		return "";

	XMLNode *node = popupMessagesNode->FirstChildElement(msgID.c_str());
	if (!node)
		return "";

	XMLNode *sn = node->FirstChild();
	if (!sn)
		return "";
		
	const char *val = sn->Value();
	if (!val || !(*val))
		return "";
	
	return val;
}
