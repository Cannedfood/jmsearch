#!/bin/makepkg

pkgname='jmsearch-git'
pkgdesc='A command line japanese-english dictionary using the JMdict file'
pkgver='0.2'
pkgrel=1
license=('')

arch=(
	'i686'
	'x86_64'
	# Not tested on arm
)

depends=('zlib')

provides=('jmsearch')


source=(
	"jmsearch::git+https://github.com/Cannedfood/jmsearch"
	'ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz'
)

md5sums=(
	'SKIP' # git -> unecessary
	'SKIP' # JMdict_e.gz : Constantly changes
)


pkgver() {
    cd "${srcdir}/jmsearch"
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
	mv 'JMdict' 'JMdict.xml' # Mostly for tests as it is extracted anyways
}

build() {
	cd "${srcdir}"
	c++ ${CXXFLAGS} -O2 -std=c++14 "jmsearch/src/"*.cpp ${LDFLAGS} -ldl -o jmsearch_binary
}

check() {
	cd "$srcdir"

	# Testing if the command crashes on lookup
	printf "kirin\n5\nbicycle\n3" | ./jmsearch_binary > /dev/null
}

package() {
	install -Dm644 "JMdict.gz" "${pkgdir}/usr/share/jmsearch/JMdict.gz"
	cd "${srcdir}"
	install -Dm755 "jmsearch_binary" "${pkgdir}/usr/bin/jmsearch"
}
