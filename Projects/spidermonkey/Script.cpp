#include "jsapi.h"
#include "Scripting.h"

void MyFinalize(JSFreeOp *fop, JSObject *obj){}

JSContext *cx;
JSRuntime *rt;
JSObject *global;

/* The class of the global object. */
/* The class of the global object. */
static JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, MyFinalize,
    JSCLASS_NO_OPTIONAL_MEMBERS
};


/* The error reporter callback. */
void reportError(JSContext *cx, const char *message, JSErrorReport *report) {
     fprintf(stderr, "%s:%u:%s\n",
             report->filename ? report->filename : "[no filename]",
             (unsigned int) report->lineno,
             message);
}

string EvaluateScript(const string& script) {
    
	/* Your application code here. This may include JSAPI calls to create your own custom JS objects and run scripts. */
	//char *script = "var o = {\"first_name\": \"matt\", \"last_name\":\"Jennings\", \"company\": \"walmart\"}; function getString(o){ if ( 'company' in o ) return o.company; else return o.first_name + \" \" + o.last_name; } getString(o);";
    jsval rval;
    JSString *str;
    JSBool ok;
    const char *filename = "noname";
    unsigned int lineno = 0;

	ok = JS_EvaluateScript(cx, global, script.c_str(), strlen(script.c_str()),
                           filename, lineno, &rval);
   // if (rval == NULL | rval == JS_FALSE)
      //  return 1;

    str = JS_ValueToString(cx, rval);	
	JSObject* obj;

    return string(JS_EncodeString(cx, str));

}

void SetupScripting() {
    /* Initialize the JS engine -- new/required as of SpiderMonkey 31. */
    JS_Init(120000);

    /* Create a JS runtime. */
    rt = JS_NewRuntime(8L * 1024L * 1024L);
    
    /* Create a context. */
	cx = JS_NewContext(rt, 8192);
    JS_SetOptions(cx, JSOPTION_VAROBJFIX);
    JS_SetErrorReporter(cx, reportError);


	/* Enter a request before running anything in the context */
    JSAutoRequest ar(cx);

    /* Create the global object in a new compartment. */
    global = JS_NewGlobalObject(cx, &global_class, NULL);
    
    /* Set the context's global */
    JSAutoCompartment ac(cx, global);
    JS_SetGlobalObject(cx, global);

    /* Populate the global object with the standard globals, like Object and Array. */
    JS_InitStandardClasses(cx, global);
	
}

void TearDownScripting(){
    JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);

    /* Shut down the JS engine. */
    JS_ShutDown();

}