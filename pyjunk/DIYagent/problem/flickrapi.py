#!/usr/bin/env python
# -*- encoding: utf-8 -*-

"""
	flickr.py
		Copyright 2004-2006 James Clarke <james@jamesclarke.info>
		Portions Copyright 2007-2008 Joshua Henderson <joshhendo@gmail.com>
	flickrapi
		Copyright (c) 2007 by the respective coders, see
		http://www.stuvel.eu/projects/flickrapi

	Modified & Simplified by Amb @ 2012-04-11
"""

from urllib import urlencode
from urllib2 import urlopen, Request
from xml.dom import minidom
from multipart import Multipart, Part
import hashlib
import webbrowser
import os

HOST = 'http://flickr.com'
API = '/services/rest'

# set these here or using flickr.API_KEY in your application
API_KEY = '4b096054791f66ac2b6889b348019229'
API_SECRET = 'bd1f5b0753e89697'

# The next 2 variables are only importatnt if authentication is used

# this can be set here or using flickr.tokenPath in your application
# this is the path to the folder containing tokenFile (default: flickr.info)
tokenPath = ''

# this can be set here or using flickr.tokenFile in your application
# this is the name of the file containing the stored token.
tokenFile = 'init.info'
usertoken = None

class FlickrError(Exception): pass

class Photo(object):
	"""Represents a Flickr Photo."""

	#XXX: Hopefully None won't cause problems
	def __init__(self, id):
		"""Must specify id, rest is optional."""
		self.id = id

		self.secret = None
		self.server = None
		self.farm = None
		self._load_properties()

	def _load_properties(self):
		"""Loads the properties from Flickr."""
		
		method = 'flickr.photos.getInfo'
		data = _doget(method, photo_id=self.id)
		
		photo = data.rsp.photo

		self.secret = photo.secret
		#self.format = photo.originalformat
		self.server = photo.server
		self.farm = photo.farm

	def __str__(self):
		return '<Flickr Photo %s>' % self.id

	def getURL(self):
		return "http://farm%s.static.flickr.com/%s/%s_%s_b.jpg" % \
			(self.farm, self.server, self.id, self.secret)

class Auth():
	def getFrob(self):
		"""Returns a frob that is used in authentication"""
		method = 'flickr.auth.getFrob'
		sig_str = API_SECRET + 'api_key' + API_KEY + 'method' + method
		signature_hash = hashlib.md5(sig_str).hexdigest()
		data = _doget(method, auth=False, api_sig=signature_hash)
		return data.rsp.frob.text

	def loginLink(self, permission, frob):
		"""Generates a link that the user should be sent to"""
		sig_str = API_SECRET + 'api_key' + API_KEY + 'frob' + frob + 'perms' + permission
		signature_hash = hashlib.md5(sig_str).hexdigest()
		perms = permission
		link = "http://flickr.com/services/auth/?api_key=%s&perms=%s&frob=%s&api_sig=%s" % (API_KEY, perms, frob, signature_hash)
		return link

	def getToken(self, frob):
		"""This token is what needs to be used in future API calls"""
		method = 'flickr.auth.getToken'
		sig_str = API_SECRET + 'api_key' + API_KEY + 'frob' + frob + 'method' + method
		signature_hash = hashlib.md5(sig_str).hexdigest()
		data = _doget(method, auth=False, api_sig=signature_hash, 
					  api_key=API_KEY, frob=frob)
		return data.rsp.auth.token.text



#useful methods

def _doget(method, auth=False, **params):
	"""uncomment to check you aren't killing the flickr server
	print "***** do get %s" % method"""

	#Convert lists to strings with ',' between items.
	for (key, value) in params.items():
		if isinstance(value, list):
			params[key] = ','.join([item for item in value])

	url = '%s%s/?api_key=%s&method=%s&%s%s'% \
		  (HOST, API, API_KEY, method, urlencode(params),
				  _get_auth_url_suffix(method, auth, params))
	
	#print url
	return _get_data(minidom.parse(urlopen(url)))

def _get_data(xml):
	"""Given a bunch of XML back from Flickr, we turn it into a data structure
	we can deal with (after checking for errors)."""
	data = unmarshal(xml)
	if not data.rsp.stat == 'ok':
		msg = "ERROR [%s]: %s" % (data.rsp.err.code, data.rsp.err.msg)
		raise FlickrError, msg
	return data

def _get_api_sig(params):
	"""Generate API signature."""
	token = userToken()
	parameters = ['api_key', 'auth_token']
	for item in params.items():
		parameters.append(item[0])
	parameters.sort()

	api_string = [API_SECRET]

	for item in parameters:
		for chocolate in params.items():
			if item == chocolate[0]:
				api_string.append(item)
				api_string.append(str(chocolate[1]))
		if item == 'api_key':
			api_string.append('api_key')
			api_string.append(API_KEY)
		if item == 'auth_token':
			api_string.append('auth_token')
			api_string.append(token)	

	return hashlib.md5(''.join(api_string)).hexdigest()

def _get_auth_url_suffix(method, auth, params):
	"""Figure out whether we want to authorize, and if so, construct a suitable
	URL suffix to pass to the Flickr API."""
	authentication = False

	# auth may be passed in via the API, AUTH may be set globally (in the same
	# manner as API_KEY, etc). We do a few more checks than may seem necessary
	# because we allow the 'auth' parameter to actually contain the
	# authentication token, not just True/False.
	
	if auth:
		token = userToken()
		authentication = True;

	# If we're not authenticating, no suffix is required.
	if not authentication:
		return ''

	full_params = params
	full_params['method'] = method

	return '&auth_token=%s&api_sig=%s' % (token, _get_api_sig(full_params) )

#stolen methods

class Bag(): pass

#unmarshal taken and modified from pyamazon.py
#makes the xml easy to work with
def unmarshal(element):
	rc = Bag()
	if isinstance(element, minidom.Element):
		for key in element.attributes.keys():
			setattr(rc, key, element.attributes[key].value)
			
	childElements = [e for e in element.childNodes \
					 if isinstance(e, minidom.Element)]
	if childElements:
		for child in childElements:
			key = child.tagName
			if hasattr(rc, key):
				if type(getattr(rc, key)) <> type([]):
					setattr(rc, key, [getattr(rc, key)])
				setattr(rc, key, getattr(rc, key) + [unmarshal(child)])
			elif isinstance(child, minidom.Element) and \
					 (child.tagName == 'Details'):
				# make the first Details element a key
				setattr(rc,key,[unmarshal(child)])
				#dbg: because otherwise 'hasattr' only tests
				#dbg: on the second occurence: if there's a
				#dbg: single return to a query, it's not a
				#dbg: list. This module should always
				#dbg: return a list of Details objects.
			else:
				setattr(rc, key, unmarshal(child))
	else:
		#jec: we'll have the main part of the element stored in .text
		#jec: will break if tag <text> is also present
		text = "".join([e.data for e in element.childNodes \
						if isinstance(e, minidom.Text)])
		setattr(rc, 'text', text)
	return rc


#my methods
def getToken(token = None):
	global usertoken
	if token == None:
		frob = Auth().getFrob()
		url = Auth().loginLink('write', frob)
		webbrowser.open(url)
		raw_input("press anykey after you done!")
		usertoken = Auth().getToken(frob)

		if not tokenPath == '':
			f = file(os.path.join(tokenPath,tokenFile), 'a')
		else:
			f = file(tokenFile, 'a')
		print >>f, '<flickr token=\"%s\" />' % (usertoken)
		f.close()
	else:
		usertoken = token

def userToken():
	#return the token
	if usertoken == None: getToken()
	return usertoken

def uploadpic(imgfile):
	#unload a img with multipart format
	#return a Photo Object

	url = 'http://api.flickr.com/services/upload/'
	
	values = {'api_key':API_KEY, 'auth_token':userToken(), 'api_sig':_get_api_sig({})}

	body = Multipart()

	for key, val in values.iteritems():
		part = Part({'name': key}, val)
		body.attach(part)
	
	img = open(imgfile, 'rb').read()
	imgpart = Part({'name': 'photo', 'filename': imgfile}, img, 'image/jpeg')
	body.attach(imgpart)
	
	#print >>open("uploaddata.multipart", "w"), str(body)

	request = Request(url)
	request.add_data(str(body))
	(header, value) = body.header()
	request.add_header(header, value)

	data = _get_data(minidom.parse(urlopen(request)))
	return Photo(data.rsp.photoid.text)

