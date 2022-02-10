/*
Copyright (C) 2022 marc2003
Generates ScintillaImpl.h from Scintilla.iface

Usage: node ScintillaImpl.mjs
*/

'use strict'

const filenames = {
	'iface': new URL('./Scintilla.iface', import.meta.url),
	'template': new URL('./ScintillaImpl.template.h', import.meta.url),
	'output': new URL('./ScintillaImpl.h', import.meta.url),
}

const options = 'utf8'
const CRLF = '\r\n'

const typeAliases = {
	'cells': 'const char*',
	'colour': 'Colour',
	'colouralpha': 'ColourAlpha',
	'findtext': 'void*',
	'formatrange': 'void*',
	'keymod': 'int',
	'line': 'Line',
	'pointer': 'void*',
	'position': 'Position',
	'string': 'const char*',
	'stringresult': 'char*',
	'textrange': 'void*',
}

const basicTypes = [
	"bool",
	"char*",
	"Colour",
	"ColourAlpha",
	"const char*",
	"int",
	"intptr_t",
	"Line",
	"Position",
	"void",
	"void*",
]

function format(name, type, ptr) {
	if (!name.length) return '0'
	if (type.endsWith('*')) return `reinterpret_cast<${ptr}>(${name})`
	if (!basicTypes.includes(type)) return `static_cast<${ptr}>(${name})`
	return name
}

function get_args(line) {
	const start = line.indexOf('(')
	const end = line.indexOf(')')
	return line.substr(start + 1, end - start - 1).split(',').map(item => item.trim().split(' '))
}

function get_type(type) {
	if (!type) return ''
	return typeAliases[type] || type
}

function create_body(content) {
	const lines = content.split(CRLF)
	const features = ['fun', 'get', 'set']
	let tmp = []

	for (const line of lines) {
		if (line.startsWith('cat Deprecated')) break

		const arr = line.substr(0, line.indexOf('=')).split(' ')
		const feature = arr[0]

		if (features.includes(feature)) {
			const ret = get_type(arr[1])
			const name = arr[2]

			const [wp, lp] = get_args(line)
			const typeWp = get_type(wp[0])
			const nameWp = wp[1] || ''
			const typeLp = get_type(lp[0])
			const nameLp = lp[1] || ''

			let str = `\t${ret} ${name}(`
			if (typeWp.length) str += `${typeWp} ${nameWp}`
			if (typeWp.length && typeLp.length) str += ', '
			if (typeLp.length) str += `${typeLp} ${nameLp}`
			str += ') { '

			let call;
			if (typeLp == 'const char*') {
				call = `CallString(Message::${name}, ${format(nameWp, typeWp, 'uintptr_t')}, ${nameLp})`
			} else if (typeLp.endsWith('*')) {
				call = `CallPointer(Message::${name}, ${format(nameWp, typeWp, 'uintptr_t')}, ${nameLp})`
			} else if (typeLp.length) {
				call = `Call(Message::${name}, ${format(nameWp, typeWp, 'uintptr_t')}, ${format(nameLp, typeLp, 'intptr_t')})`
			} else if (typeWp.length) {
				call = `Call(Message::${name}, ${format(nameWp, typeWp, 'uintptr_t')})`
			} else {
				call = `Call(Message::${name})`
			}

			if (ret == 'void*') str += `return reinterpret_cast<void*>(${call})`
			else if (!basicTypes.includes(ret)) str += `return static_cast<${ret}>(${call})`
			else if (ret != 'void') str += `return ${call}`
			else str += call

			str += '; }'
			tmp.push(str)
		}
	}
	tmp.sort()
	return tmp.join(CRLF)
}

import { readFile, writeFile } from 'fs/promises'
const template = await readFile(filenames.template, options)
const iface = await readFile(filenames.iface, options)
await writeFile(filenames.output, template.replace('REPLACEME', create_body(iface)), options)
console.log('Done!')
