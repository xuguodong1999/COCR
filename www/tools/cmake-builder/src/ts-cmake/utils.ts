import * as path from 'path';

function convertCMakePath(s: string) {
    if (path.sep === '/') {
        return s;
    }
    return s.split(path.sep).join('/');
}

export function appendCmd(cmd: string, frag: string, split = ' ') {
    return convertCMakePath(`${cmd}${split}${frag}`);
}

function naiveDeepClone<T>(t: T): T {
    if (undefined === t) {
        return t;
    }
    return JSON.parse(JSON.stringify(t));
}

type NonUndefined<T> = T extends undefined ? never : T;

// difference vs Object.assign:
// 1、deep copy instead of shadow copy
// 2、force keep node structure, only assign leaf value(to avoid runtime exception for 123.memberName)
//     const obj1={a:{aa:1},b:2};
//     const obj2={a:1,b:3,c:4};
//     Object.assign(obj1,obj2)==={a:1,b:3,c:4}
//     mergeBIntoAByKey(obj1,obj2)==={a:{aa:1},b:3,c:4}
// in above case, "a" will never be 1 so call to "a.aa" is always safe
// 3、join array instead of override
export function mergeBIntoAByKey<Ta, Tb extends Partial<Ta>>(a: NonUndefined<Ta>, b: Tb): NonUndefined<Ta> {
    const newA = naiveDeepClone(a);
    if (isJsonObject(newA)) {
        const newB = naiveDeepClone(b);
        mergeBIntoAByKeyImpl(Object(newA), newB);
    }
    return newA;
}

function isJsonObject(o: any) {
    return null !== o && !Array.isArray(o) && 'object' === typeof o;
}

function isValidLeafType(o: any) {
    switch (typeof o) {
        case 'string':
        case 'number':
        case 'boolean':
        case 'undefined':
            return true;
    }
    return Array.isArray(o) || null === o;
}

// keepStructA = true: keep a's runtime type
// keepStructA = false: extend a's runtime type to union(a,b)
function mergeBIntoAByKeyImpl<Ta extends {}, Tb>(a: NonUndefined<Ta>, b: Tb, keepStructA = false) {
    for (const key of Object.keys(Object(b))) {
        // @ts-ignore
        const aKey = a[key];
        // @ts-ignore
        const bKey = b[key];
        if (keepStructA) {
            if (isJsonObject(aKey)) {
                mergeBIntoAByKeyImpl(aKey, bKey, keepStructA);
            } else if (Array.isArray(aKey) && Array.isArray(bKey)) {
                // @ts-ignore
                a[key] = [...aKey, ...bKey];
            } else if (isValidLeafType(aKey)) {
                // @ts-ignore
                a[key] = bKey;
            }
        } else {
            if (isJsonObject(aKey)) {
                mergeBIntoAByKeyImpl(aKey, bKey, keepStructA);
            } else if (Array.isArray(aKey) && Array.isArray(bKey)) {
                // @ts-ignore
                a[key] = [...aKey, ...bKey];
            } else if (isValidLeafType(aKey)) {
                // @ts-ignore
                a[key] = bKey;
            }
        }
    }
}
