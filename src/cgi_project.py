#!/usr/bin/python
import ConfigParser
import optparse

def main ():
	"""Cria a configuração de um novo projeto"""
	p = optparse.OptionParser ()
	p = optparse.OptionParser (description = 'Cria e altera o arquivo de configuração de um novo projeto',
									   prog='cgi_project',
									   version='cgi_project 0.1'
										usage= '%prog command')
	
	options, arguments = p.parse_args()
	