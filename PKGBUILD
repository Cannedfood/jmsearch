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
)

md5sums=(
	'SKIP' # git -> unecessary
)


pkgver() {
    cd "${srcdir}/jmsearch" || exit
    printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
	cd "${srcdir}" || exit

	if [ -f '/usr/share/jmsearch/JMdict.gz' ]
	then
		echo "Dictionary file already found; Using it to minimize traffic."
		cp '/usr/share/jmsearch/JMdict.gz' ./
	fi

	wget -q --show-progress --passive-ftp -c -O JMdict.gz 'ftp://ftp.monash.edu.au/pub/nihongo/JMdict.gz'
}

build() {
	cd "${srcdir}" || exit
	c++ ${CXXFLAGS} -O2 -std=c++14 "jmsearch/src/"*.cpp ${LDFLAGS} -ldl -o jmsearch_binary
}

check() {
	cd "$srcdir" || exit

	# Testing if the command crashes on lookup
	printf "kirin\n5\nbicycle\n3" | ./jmsearch_binary > /dev/null
}

package() {
	cd "${srcdir}" || exit
	install -Dm644 "JMdict.gz" "${pkgdir}/usr/share/jmsearch/JMdict.gz"
	install -Dm755 "jmsearch_binary" "${pkgdir}/usr/bin/jmsearch"
}
