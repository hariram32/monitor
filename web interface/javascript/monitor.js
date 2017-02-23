function refresh_grid( )
{
	//window.alert("Refreshing");
	var xhttp = new XMLHttpRequest( );
	xhttp.onreadystatechange = function( )
	{
		if ( xhttp.readyState == 4 && xhttp.status == 200 ) 
		{
			//window.alert("Ready state: " + xhttp.readyState);
			document.getElementById( "grid" ).innerHTML = xhttp.responseText;
			$(function () { $("[data-toggle='popover']").popover(); });
		}
		else
		{
			//window.alert("Ready state: " + xhttp.readyState);
		}
	};
	xhttp.open("GET", "php/grid.php", true);
	xhttp.send();
}
