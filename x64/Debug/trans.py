
import json
import urllib.request
import urllib.parse
import sys

def translate_Youdao(content):
    url = "http://fanyi.youdao.com/translate?smartresult=dict&smartresult=rule"

    head = {}
    head['User-Agent'] = 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36'

    data = {}
    data['i'] = content
    data['from'] = 'AUTO'
    data['to'] = 'AUTO'
    data['smartresult'] = 'dict'
    data['client'] = 'fanyideskweb'
    data['salt'] = '1514961495816'
    data['sign'] = '4712476550b84a8e74f9c7848719933a'
    data['doctype'] = 'json'
    data['version'] = '2.1'
    data['keyfrom'] = 'fanyi.web'
    data['action'] = 'FY_BY_CLICKBUTTION'
    data['typoResult'] = 'false'

    data = urllib.parse.urlencode(data).encode('utf-8')

    request = urllib.request.Request(url, data, head)

    response = urllib.request.urlopen(request)
    html = response.read().decode('utf-8')
    target = json.loads(html)

    print(str(target['translateResult'][0][0]['tgt']))
    return str(target['translateResult'][0][0]['tgt'])

#str = translate_Youdao('three')
#print(str)
