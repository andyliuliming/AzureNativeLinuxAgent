#pragma once
#include <cstdio>

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
//#ifndef _WIN32
//#include <sys/param.h>
//#endif
//#ifdef _WIN32
////windows code goes here
//#define POPEN _popen
//#define PCLOSE _pclose
//#define SLEEP _sleep
//#define SOCKET_OPTION_P (const char*)
//#define SEND_TO_OPTION (const char*)
//#define RECV_TO_OPTION (char*)
//#define CLOSESOCKET closesocket
//#define SHUT_RDWR SD_BOTH
//#else
#include <unistd.h>
//linux code goes here
#define POPEN popen
#define PCLOSE pclose
#define SLEEP(x) usleep(x*1000)
#define SOCKET_OPTION_P
#define SEND_TO_OPTION
#define RECV_TO_OPTION
#define CLOSESOCKET close
//#endif

//#ifdef _WIN32
//#define RANDOM() rand()
#ifdef __FreeBSD__
#define RANDOM() random()
#else
#define RANDOM() rand()
#endif

#define CERTIFICATIONS_XML_FILE_NAME ("Native_Certificates.xml")
#define CERTIFICATIONS_FILE_NAME ("Native_Certificates.pem")
#define HOSTING_ENVIRONMENT_CONFIG ("Native_HostingEnvironmentConfig.xml")
#define PROTECTED_SETTINGS_CERTIFICATE_FILE_NAME ("Native_Certificates.p7m")
#define OVF_ENV_FILE_NAME ("Native_ovf-env.xml")
#define OVF_ENV_FILE_FULL_PATH ("/mnt/cdrom/secure/ovf-env.xml")
#define SECURE_MOUNT_POINT ("/mnt/cdrom/secure")
#define SHARED_CONFIG_FILE_NAME ("Native_SharedConfig.xml");
#define TRANSPORT_CERT_CIPHER_NAME ("DES_EDE3_CBC")
#define TRANSPORT_CERT_PRIV ("Native_TransportPrivate.pem")
#define TRANSPORT_CERT_PUB ("Native_TransportCert.pem")
#define VMM_CONFIG_FILE_NAME ("linuxosconfiguration.xml")
#define VMM_CONFIG_FILE_FULL_PATH ("/mnt/cdrom/secure/linuxosconfiguration.xml")
#define WAAGENT_LIB_BASE_DIR  ("/var/lib/waagent/")
#define PROVISIONED_FILE_PATH ("/var/lib/waagent/provisioned")
#define WAAGENT_NAME ("AzureNativeLinuxAgent")
#define WAAGENT_PID_FILE ("/var/run/waagent.pid")
#define WAAGENT_SYSLOG_NAME ("AzureNativeLinuxAgent")
#define WAAGENT_VERSION ("2012-11-30")
#define WORKING_DIR ("/var/lib/waagent")
#define EXTENSION_LOG_BASE_DIR ("/var/log/azure/")
#define STORAGE_VERSION ("2014-02-14")
#define BSD_SUDOERS_DIR_BASE ("/usr/local/etc/")
#define AGENT_SUCCESS (0)
#define AGENT_FAILED (1)
#define PROCESS_EXIT_SUCCESS (0)
#define ELEMENT_NOT_FOUNT (404)
#define OVF_VERSION_NOT_MATCH (2)

typedef unsigned char       BYTE, *PBYTE;
typedef unsigned char       UINT8, *PUINT8;


#define DHCPDISCOVER (1)
#define DHCP_UDP_PORT (68)
#define DHCP_UDP_TO_PORT (67)