var incX = 0;
var incY = 0;
var intervalID = -1;
var goToURL = "#";

function scrollWin(modelValues) {

	if (modelValues != ''){
		var diagram = document.getElementById("diagram");
		var xOffset = findPosX(diagram);
		var yOffset = findPosY(diagram);
	
		var viewportWidth = 0, viewportHeight = 0;
		if( typeof( window.innerWidth ) == 'number' ) {
			viewportWidth = window.innerWidth;
			viewportHeight = window.innerHeight;
		} else if( document.documentElement && ( document.documentElement.clientWidth || document.documentElement.clientHeight ) ) {
			viewportWidth = document.documentElement.clientWidth;
			viewportHeight = document.documentElement.clientHeight;
		} else if( document.body && ( document.body.clientWidth || document.body.clientHeight ) ) {
			viewportWidth = document.body.clientWidth;
			viewportHeight = document.body.clientHeight;
		}
		
		var xyValueArray = modelValues.split(",");
		var x = xyValueArray[0];
		var y = xyValueArray[1];
		var w = xyValueArray[2];
		var h = xyValueArray[3];
		
		x = x*1 + xOffset*1 - viewportWidth/2 + w/2;
		y = y*1 + yOffset*1 - viewportHeight/2 + h/2;
	
		incX = incY = 2;
		
		if (document.all){
			if (!document.documentElement.scrollLeft)
				incX *= (document.body.scrollLeft > x?-15:15);
			else
				incX *= (document.documentElement.scrollLeft > x?-15:15);
	               
			if (!document.documentElement.scrollTop)
				incY *= (document.body.scrollTop > y?-15:15);
			else
				incY *= (document.documentElement.scrollTop > y?-15:15);
	
		}else{
			incX *= (window.pageXOffset > x?-15:15);
			incY *= (window.pageYOffset > y?-15:15);
		}
		
		intervalID = setInterval("doScroll(" + x + ", " + y +")", 1);
	}	
}

function doScroll(x, y) {
	var beforeX;
	var beforeY;
	var afterX;
	var afterY;

	if (document.all){
		if (!document.documentElement.scrollLeft)
			beforeX = document.body.scrollLeft;
		else
			beforeX = document.documentElement.scrollLeft;
               
		if (!document.documentElement.scrollTop)
			beforeY = document.body.scrollTop;
		else
			beforeY = document.documentElement.scrollTop;
			
	}else{
		beforeX = window.pageXOffset;
		beforeY = window.pageYOffset;
	}

	window.scrollTo(beforeX+incX, beforeY+incY);
  
  	if (document.all){
		if (!document.documentElement.scrollLeft)
			afterX = document.body.scrollLeft;
		else
			afterX = document.documentElement.scrollLeft;
               
		if (!document.documentElement.scrollTop)
			afterY = document.body.scrollTop;
		else
			afterY = document.documentElement.scrollTop;
			
	}else{
		afterX = window.pageXOffset;
		afterY = window.pageYOffset;
	}
  
	if (incX!=0 && beforeX==afterX)
		incX = 0;
	if (incY!=0 && beforeY==afterY)
		incY = 0;
  
	if ((incX < 0 && (afterX < x || afterX+incX < x)) || (incX > 0 && (afterX > x || afterX+incX > x))) {
		incX = 0;
	} if ((incY < 0 && (afterY < y || afterY+incY < y)) || (incY > 0 && (afterY > y || afterY+incY > y))) {
		incY = 0;
	}

	if (incX==0 && incY==0) {
    		window.clearInterval(intervalID);
	}
}

function findPosX(obj)
{
	var curleft = 0;
	if (obj.offsetParent)
	{
		while (obj.offsetParent)
		{
			curleft += obj.offsetLeft
			obj = obj.offsetParent;
		}
	}
	else if (obj.x)
		curleft += obj.x;
	return curleft;
}

function findPosY(obj)
{
	var curtop = 0;
	if (obj.offsetParent)
	{
		while (obj.offsetParent)
		{
			curtop += obj.offsetTop
			obj = obj.offsetParent;
		}
	}
	else if (obj.y)
		curtop += obj.y;
	return curtop;
}

function showSpotLight(modelValues) {
	if (modelValues != ''){
		var diagram = document.getElementById("diagram");
		var xOffset = findPosX(diagram);
		var yOffset = findPosY(diagram);
		
		var xyValueArray = modelValues.split(",");
		var shapeX = xyValueArray[0]*1;
		var shapeY = xyValueArray[1]*1;
		var x = shapeX + xOffset*1 - 10;
		var y = shapeY + yOffset*1 - 10;
		var w = xyValueArray[2]*1 + 20;
		var h = xyValueArray[3]*1 + 20;
		var url = xyValueArray[4];
		
		var spotLight = document.getElementById("spotlight");
		var spotLightResourcesTop = document.getElementById("spotLightResourcesTop");
		var spotLightResourcesRight = document.getElementById("spotLightResourcesRight");
		var spotLightTable = document.getElementById("spotLightTable");
		var spotLightCell = document.getElementById("spotLightCell");
		spotLight.style.widht = w;
		spotLight.style.height = h;
		goToURL = url;
	
		N = (document.all) ? 0 : 1;
		if (N) {
			spotLight.style.left = x;
			spotLight.style.top = y;
			spotLightResourcesTop.style.left = x + w - 50;
			spotLightResourcesTop.style.top = y - 25;
			spotLightResourcesRight.style.left = x + w;
			spotLightResourcesRight.style.top = y + 10;
			spotLightTable.style.width = w;
			spotLightCell.style.width = w
			spotLightCell.style.height = h;
		} else {
			spotLight.style.posLeft = x;
			spotLight.style.posTop = y;
			spotLightResourcesTop.style.posLeft = x + w - 50;
			spotLightResourcesTop.style.posTop = y - 25;
			spotLightResourcesRight.style.posLeft = x + w;
			spotLightResourcesRight.style.posTop = y + 10;
			spotLightTable.style.width = w;
			spotLightCell.style.width = w;
			spotLightCell.style.height = h;
		}
		
		spotLight.style.visibility = "visible"
		if (shapeY < 40){
			spotLightResourcesRight.style.visibility = "visible"
			spotLightResourcesTop.style.visibility = "hidden"
		}else{
			spotLightResourcesRight.style.visibility = "hidden"
			spotLightResourcesTop.style.visibility = "visible"
		}
			
	}
}

function clearSpotLight() {
	var spotLight = document.getElementById("spotlight");
	var spotLightResourcesTop = document.getElementById("spotLightResourcesTop");
	var spotLightResourcesRight = document.getElementById("spotLightResourcesRight");
	spotLight.style.visibility = "hidden"
	spotLightResourcesTop.style.visibility = "hidden"
	spotLightResourcesRight.style.visibility = "hidden"
	document.location.href = goToURL;
}