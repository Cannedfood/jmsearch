#include "Annotation.hpp"

Annotations::Annotations() {
#	define ANNOTATION(NAME, ENUMERATION) mNames[ENUMERATION] = NAME

#	undef ANNOTATION
}

Annotations::Annotation Annotations::parseAnnotation(const char* value) {
	return mReverseName[value];
}

const char* Annotations::annotationString(Annotations::Annotation n) const {
	return mNames[n];
}

const char* Annotations::annotationMeaning(Annotations::Annotation n) const {
	return mDescriptions[n];
}
