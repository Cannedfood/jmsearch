pkgname='jmsearch'
pkgdesc='A command line japanese-english dictionary using the JMdict file'
pkgver='0.1'
pkgrel=1
license=('')

arch=(
	'i686'
	'x86_64'
	# Not tested on arm
)

source=(
	'git+ssh://git@github.com/Cannedfood/jmsearch.git'
	'ftp://ftp.monash.edu.au/pub/nihongo/JMdict_e.gz'
)

md5sums=(
	'SKIP'
	'f41db49f3a5e51b5fe96530355faa38f'
)


prepare() {
	mv 'JMdict_e' 'JMdict_e.xml'
}

build() {
	cd "$srcdir"
	c++ -std=c++14 "jmsearch/src/"*.cpp -o jmsearch_binary
}

check() {
	cd "$srcdir"

	# Testing if the command crashes on lookup
	printf "kirin\n5\n.the\n3"
	printf "kirin\n5\n.the\n3" | ./jmsearch_binary > /dev/null
}

package() {
	cd "${srcdir}"
	install -Dm644 "JMdict_e.xml" "${pkgdir}/usr/share/${pkgname}/JMdict_e.xml"
	install -Dm755 "jmsearch_binary" "${pkgdir}/usr/bin/${pkgname}"
}
