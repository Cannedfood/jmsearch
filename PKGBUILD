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

provides=('jmsearch')


source=(
	"jmsearch::git+https://github.com/Cannedfood/jmsearch"
	'ftp://ftp.monash.edu.au/pub/nihongo/JMdict_e.gz'
)

md5sums=(
	'SKIP' # git -> unecessary
	'SKIP' # JMdict_e.gz : Constantly changes
)


pkgver() {
        cd "$srcdir/jmsearch"
        printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
	mv 'JMdict_e' 'JMdict_e.xml'
}

build() {
	cd "$srcdir"
	c++ ${CXXFLAGS} -O2 -std=c++14 "jmsearch/src/"*.cpp ${LDFLAGS} -o jmsearch_binary
}

check() {
	cd "$srcdir"

	# Testing if the command crashes on lookup
	printf "kirin\n5\nbicycle\n3" | ./jmsearch_binary > /dev/null
}

package() {
	cd "${srcdir}"
	install -Dm644 "JMdict_e.xml" "${pkgdir}/usr/share/${pkgname}/JMdict_e.xml"
	install -Dm755 "jmsearch_binary" "${pkgdir}/usr/bin/${pkgname}"
}
